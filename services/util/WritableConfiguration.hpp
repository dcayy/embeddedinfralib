#ifndef SERVICES_MEMORY_MAPPED_STATIC_CONFIGURATION_HPP
#define SERVICES_MEMORY_MAPPED_STATIC_CONFIGURATION_HPP

#include "hal/interfaces/Flash.hpp"
#include "infra/stream/ByteInputStream.hpp"
#include "infra/stream/ByteOutputStream.hpp"
#include "infra/syntax/ProtoFormatter.hpp"
#include "infra/syntax/ProtoParser.hpp"
#include "mbedtls/sha256.h"
#include "infra/event/EventDispatcher.hpp"

namespace services
{
    template<class T>
    class WritableConfigurationWriter
    {
    public:
        virtual void Write(const T& newValue, const infra::Function<void()>& onDone) = 0;
    };

    template<class TRef>
    class WritableConfigurationReader
    {
    public:
        virtual bool Valid() const = 0;
        virtual const TRef& Get() const = 0;
        virtual void Read(const infra::Function<void()>& onDone) = 0;
    };

    template<class T, class TRef>
    class WritableConfigurationReaderWriter
        : public WritableConfigurationWriter<T>
        , public WritableConfigurationReader<TRef>
    {
    public:
        virtual bool Valid() const = 0;
        virtual const TRef& Get() const = 0;
        virtual void Read(const infra::Function<void()>& onDone) = 0;
        virtual void Write(const T& newValue, const infra::Function<void()>& onDone) = 0;
    };

    template<class T, class TRef>
    class WritableConfiguration
        : public WritableConfigurationReaderWriter<T, TRef>
    {
    public:
        WritableConfiguration(hal::Flash& flash);

        virtual bool Valid() const override;
        virtual const TRef& Get() const override;

    protected:
        void LoadConfiguration(infra::ConstByteRange memory);
        void WriteConfiguration(const T& newValue, infra::DataOutputStream::WithWriter<infra::ByteOutputStreamWriter>& stream, infra::ByteRange storage, const infra::Function<void()>& onDone);

    protected:
        struct Header
        {
            std::array<uint8_t, 8> hash;
            uint32_t size;
        };

    protected:
        hal::Flash& flash;
        TRef value;
        bool valid = false;

        infra::Function<void()> onDone;
    };

    template<class T, class TRef>
    class MemoryMappedWritableConfiguration
        : public WritableConfiguration<T, TRef>
    {
    public:
        MemoryMappedWritableConfiguration(hal::Flash& flash, infra::ConstByteRange memory);

        virtual void Read(const infra::Function<void()>& onDone) override;

        // Warning: Allocates memory on the heap!
        virtual void Write(const T& newValue, const infra::Function<void()>& onDone) override;

    private:
        std::shared_ptr<infra::DataOutputStream::WithWriter<infra::ByteOutputStreamWriter>> stream;

    private:
        infra::ConstByteRange memory;
        infra::Function<void()> onWriteDone;
    };

    template<class T, class TRef>
    class FlashReadingWritableConfiguration
        : public WritableConfiguration<T, TRef>
    {
    public:
        FlashReadingWritableConfiguration(hal::Flash& flash);

        virtual void Read(const infra::Function<void()>& onDone) override;
        virtual void Write(const T& newValue, const infra::Function<void()>& onDone) override;

    private:
        std::array<uint8_t, T::maxMessageSize + sizeof(typename WritableConfiguration<T, TRef>::Header)> memory;
        infra::ByteOutputStream stream;
    };

    ////    Implementation    ////

    template<class T, class TRef>
    WritableConfiguration<T, TRef>::WritableConfiguration(hal::Flash& flash)
        : flash(flash)
    {}

    template<class T, class TRef>
    bool WritableConfiguration<T, TRef>::Valid() const
    {
        return valid;
    }

    template<class T, class TRef>
    const TRef& WritableConfiguration<T, TRef>::Get() const
    {
        return value;
    }

    template<class T, class TRef>
    void WritableConfiguration<T, TRef>::LoadConfiguration(infra::ConstByteRange memory)
    {
        infra::ByteInputStream::WithReader stream(memory, infra::noFail);
        auto header = stream.Extract<Header>();

        valid = false;
        if (header.size + sizeof(Header) <= memory.size())
        {
            std::array<uint8_t, 32> messageHash;
            mbedtls_sha256(memory.begin() + sizeof(header.hash), std::min<std::size_t>(header.size + sizeof(header.size), memory.size() - sizeof(header.hash)), messageHash.data(), 0);

            if (infra::Head(infra::MakeRange(messageHash), sizeof(header.hash)) == header.hash)
            {
                infra::LimitedStreamReaderWithRewinding limitedReader(stream.Reader(), header.size);
                infra::DataInputStream limitedStream(limitedReader, stream.ErrorPolicy());
                infra::ProtoParser parser(limitedStream);

                value.~TRef();
                new (&value) TRef(parser);

                valid = !stream.Failed();
            }
        }
    }

    template<class T, class TRef>
    void WritableConfiguration<T, TRef>::WriteConfiguration(const T& newValue, infra::DataOutputStream::WithWriter<infra::ByteOutputStreamWriter>& stream, infra::ByteRange storage, const infra::Function<void()>& onDone)
    {
        this->onDone = onDone;
        auto headerProxy = stream.Writer().template Reserve<Header>(stream.ErrorPolicy());
        auto marker = stream.SaveMarker();
        infra::ProtoFormatter formatter(stream);
        newValue.Serialize(formatter);

        Header header;
        header.size = stream.SaveMarker() - marker;
        headerProxy = header;
        std::array<uint8_t, 32> messageHash;
        mbedtls_sha256(storage.begin() + sizeof(Header::hash), std::min<std::size_t>(header.size + sizeof(Header::size), storage.size() - sizeof(Header::hash)), messageHash.data(), 0);
        infra::Copy(infra::Head(infra::MakeRange(messageHash), sizeof(Header::hash)), infra::MakeRange(header.hash));
        headerProxy = header;

        flash.EraseAll([this, &stream]() { flash.WriteBuffer(stream.Writer().Processed(), 0, [this]() { this->Read(this->onDone); }); });
    }

    template<class T, class TRef>
    MemoryMappedWritableConfiguration<T, TRef>::MemoryMappedWritableConfiguration(hal::Flash& flash, infra::ConstByteRange memory)
        : WritableConfiguration<T, TRef>(flash)
        , memory(memory)
    {
        this->LoadConfiguration(memory);
    }

    template<class T, class TRef>
    void MemoryMappedWritableConfiguration<T, TRef>::Read(const infra::Function<void()>& onDone)
    {
        this->onDone = onDone;
        infra::EventDispatcher::Instance().Schedule([this]()
        {
            this->LoadConfiguration(memory);
            this->onDone();
        });
    }

    template<class T, class TRef>
    void MemoryMappedWritableConfiguration<T, TRef>::Write(const T& newValue, const infra::Function<void()>& onDone)
    {
        this->onWriteDone = onDone;
        auto stream = std::make_shared<infra::ByteOutputStream::WithStorage<T::maxMessageSize + sizeof(typename WritableConfiguration<T, TRef>::Header)>>();
        this->stream = stream;
        this->WriteConfiguration(newValue, *this->stream, stream->Storage(), [this]() { this->stream == nullptr; this->onWriteDone(); });
    }

    template<class T, class TRef>
    FlashReadingWritableConfiguration<T, TRef>::FlashReadingWritableConfiguration(hal::Flash& flash)
        : WritableConfiguration<T, TRef>(flash)
        , stream(memory)
    {}

    template<class T, class TRef>
    void FlashReadingWritableConfiguration<T, TRef>::Read(const infra::Function<void()>& onDone)
    {
        this->onDone = onDone;
        this->flash.ReadBuffer(memory, 0, [this]()
        {
            this->LoadConfiguration(memory);
            this->onDone();
        });
    }

    template<class T, class TRef>
    void FlashReadingWritableConfiguration<T, TRef>::Write(const T& newValue, const infra::Function<void()>& onDone)
    {
        this->WriteConfiguration(newValue, this->stream, this->memory, onDone);
    }
}

#endif