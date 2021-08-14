#include "generated/proto_cpp/EchoAttributes.pb.h"
#include "google/protobuf/compiler/cpp/cpp_helpers.h"
#include "google/protobuf/stubs/strutil.h"
#include "protobuf/protoc_echo_plugin/EchoObjects.hpp"

namespace application
{
    namespace
    {
        std::string QualifiedName(const google::protobuf::Descriptor& descriptor)
        {
            std::string namespaceString;

            namespaceString = descriptor.file()->package() + "::";
            for (auto containingType = descriptor.containing_type(); containingType != nullptr; containingType = containingType->containing_type())
                namespaceString += containingType->name() + "::";

            return namespaceString + descriptor.name();
        }

        std::string QualifiedName(const google::protobuf::EnumDescriptor& descriptor)
        {
            std::string namespaceString;

            namespaceString = descriptor.file()->package() + "::";
            for (auto containingType = descriptor.containing_type(); containingType != nullptr; containingType = containingType->containing_type())
                namespaceString += containingType->name() + "::";

            return namespaceString + descriptor.name();
        }

        std::string QualifiedDetailName(const google::protobuf::EnumDescriptor& descriptor)
        {
            std::string namespaceString;

            namespaceString = descriptor.file()->package() + "::";

            if (descriptor.containing_type() != nullptr)
            {
                namespaceString += "detail::";

                for (auto containingType = descriptor.containing_type(); containingType != nullptr; containingType = containingType->containing_type())
                    namespaceString += containingType->name();
            }

            return namespaceString + descriptor.name();
        }

        std::string QualifiedReferenceName(const google::protobuf::Descriptor& descriptor)
        {
            std::string namespaceString;

            namespaceString = descriptor.file()->package() + "::";
            for (auto containingType = descriptor.containing_type(); containingType != nullptr; containingType = containingType->containing_type())
                namespaceString += containingType->name() + "Reference::";

            return namespaceString + descriptor.name() + "Reference";
        }

        std::string QualifiedDetailName(const google::protobuf::Descriptor& descriptor)
        {
            std::string namespaceString;

            namespaceString = descriptor.file()->package() + "::";

            if (descriptor.containing_type() != nullptr)
            {
                namespaceString += "detail::";

                for (auto containingType = descriptor.containing_type(); containingType != nullptr; containingType = containingType->containing_type())
                    namespaceString += containingType->name();
            }

            return namespaceString + descriptor.name();
        }

        std::string QualifiedDetailReferenceName(const google::protobuf::Descriptor& descriptor)
        {
            std::string namespaceString;

            namespaceString = descriptor.file()->package() + "::";

            if (descriptor.containing_type() != nullptr)
            {
                namespaceString += "detail::";

                for (auto containingType = descriptor.containing_type(); containingType != nullptr; containingType = containingType->containing_type())
                    namespaceString += containingType->name();
            }

            return namespaceString + descriptor.name() + "Reference";
        }
    }

    uint32_t MaxVarIntSize(uint64_t value)
    {
        uint32_t result = 1;

        while (value > 127)
        {
            value >>= 7;
            ++result;
        }

        return result;
    }

    EchoField::EchoField(const google::protobuf::FieldDescriptor& descriptor)
        : name(descriptor.name())
        , number(descriptor.number())
        , constantName(google::protobuf::compiler::cpp::FieldConstantName(&descriptor))
    {}

    std::shared_ptr<EchoField> EchoField::GenerateField(const google::protobuf::FieldDescriptor& fieldDescriptor, EchoRoot& root)
    {
        if (fieldDescriptor.label() != google::protobuf::FieldDescriptor::LABEL_REPEATED)
            switch (fieldDescriptor.type())
            {
                case google::protobuf::FieldDescriptor::TYPE_INT64:
                    return std::make_shared<EchoFieldInt64>(fieldDescriptor);
                case google::protobuf::FieldDescriptor::TYPE_UINT64:
                    return std::make_shared<EchoFieldUint64>(fieldDescriptor);
                case google::protobuf::FieldDescriptor::TYPE_INT32:
                    return std::make_shared<EchoFieldInt32>(fieldDescriptor);
                case google::protobuf::FieldDescriptor::TYPE_FIXED64:
                    return std::make_shared<EchoFieldFixed64>(fieldDescriptor);
                case google::protobuf::FieldDescriptor::TYPE_FIXED32:
                    return std::make_shared<EchoFieldFixed32>(fieldDescriptor);
                case google::protobuf::FieldDescriptor::TYPE_BOOL:
                    return std::make_shared<EchoFieldBool>(fieldDescriptor);
                case google::protobuf::FieldDescriptor::TYPE_STRING:
                {
                    if (fieldDescriptor.options().GetExtension(string_size) != 0)
                        return std::make_shared<EchoFieldString>(fieldDescriptor);
                    else
                        return std::make_shared<EchoFieldStdString>(fieldDescriptor);
                }
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                    return std::make_shared<EchoFieldMessage>(fieldDescriptor, root);
                case google::protobuf::FieldDescriptor::TYPE_BYTES:
                    return std::make_shared<EchoFieldBytes>(fieldDescriptor);
                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                    return std::make_shared<EchoFieldUint32>(fieldDescriptor);
                case google::protobuf::FieldDescriptor::TYPE_ENUM:
                    return std::make_shared<EchoFieldEnum>(fieldDescriptor, root);
                case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
                    return std::make_shared<EchoFieldSFixed64>(fieldDescriptor);
                case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
                    return std::make_shared<EchoFieldSFixed32>(fieldDescriptor);
                default:
                    throw UnsupportedFieldType{ fieldDescriptor.name(), fieldDescriptor.type() };
            }
        else
            switch (fieldDescriptor.type())
            {
                case google::protobuf::FieldDescriptor::TYPE_STRING:
                    return std::make_shared<EchoFieldRepeated>(fieldDescriptor, std::make_shared<EchoFieldString>(fieldDescriptor));
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                    return std::make_shared<EchoFieldRepeated>(fieldDescriptor, std::make_shared<EchoFieldMessage>(fieldDescriptor, root));
                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                    return std::make_shared<EchoFieldRepeated>(fieldDescriptor, std::make_shared<EchoFieldUint32>(fieldDescriptor));
                default:
                    throw UnsupportedFieldType{ fieldDescriptor.name(), fieldDescriptor.type() };
            }
    }

    EchoEnum::EchoEnum(const google::protobuf::EnumDescriptor& descriptor)
        : descriptor(descriptor)
        , name(descriptor.name())
        , qualifiedTypeName(QualifiedName(descriptor))
        , qualifiedDetailName(QualifiedDetailName(descriptor))
    {
        for (auto containingType = descriptor.containing_type(); containingType != nullptr; containingType = containingType->containing_type())
            containedInMessageName += containingType->name();

        for (int i = 0; i != descriptor.value_count(); ++i)
            members.push_back(std::make_pair(descriptor.value(i)->name(), descriptor.value(i)->number()));
    }

    EchoMessage::EchoMessage(const google::protobuf::Descriptor& descriptor, EchoRoot& root)
        : descriptor(descriptor)
        , name(descriptor.name())
        , qualifiedName(QualifiedName(descriptor))
        , qualifiedReferenceName(QualifiedReferenceName(descriptor))
        , qualifiedDetailName(QualifiedDetailName(descriptor))
        , qualifiedDetailReferenceName(QualifiedDetailReferenceName(descriptor))
    {
        for (int i = 0; i != descriptor.enum_type_count(); ++i)
            nestedEnums.push_back(root.AddEnum(*descriptor.enum_type(i)));

        for (int i = 0; i != descriptor.nested_type_count(); ++i)
            nestedMessages.push_back(root.AddMessage(*descriptor.nested_type(i)));

        for (int i = 0; i != descriptor.field_count(); ++i)
            fields.emplace_back(EchoField::GenerateField(*descriptor.field(i), root));

        ComputeMaxMessageSize();
    }

    infra::Optional<uint32_t> EchoMessage::MaxMessageSize() const
    {
        return maxMessageSize;
    }

    void EchoMessage::ComputeMaxMessageSize()
    {
        struct NoMaxMessageSize {};
        class GenerateMaxMessageSizeVisitor
            : public EchoFieldVisitor
        {
        public:
            GenerateMaxMessageSizeVisitor(uint32_t& maxMessageSize)
                : maxMessageSize(maxMessageSize)
            {}

            virtual void VisitInt64(const EchoFieldInt64& field) override
            {
                maxMessageSize += MaxVarIntSize(std::numeric_limits<uint64_t>::max()) + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitUint64(const EchoFieldUint64& field) override
            {
                maxMessageSize += MaxVarIntSize(std::numeric_limits<uint64_t>::max()) + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitInt32(const EchoFieldInt32& field) override
            {
                maxMessageSize += MaxVarIntSize(std::numeric_limits<uint32_t>::max()) + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitFixed64(const EchoFieldFixed64& field) override
            {
                maxMessageSize += 8 + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitFixed32(const EchoFieldFixed32& field) override
            {
                maxMessageSize += 4 + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitBool(const EchoFieldBool& field) override
            {
                maxMessageSize += MaxVarIntSize(1) + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitString(const EchoFieldString& field) override
            {
                maxMessageSize += field.maxStringSize + MaxVarIntSize(field.maxStringSize) + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitStdString(const EchoFieldStdString& field) override
            {
                throw NoMaxMessageSize();
            }

            virtual void VisitMessage(const EchoFieldMessage& field) override
            {
                uint32_t fieldMaxMessageSize = 0;
                GenerateMaxMessageSizeVisitor visitor(fieldMaxMessageSize);
                for (auto& field : field.message->fields)
                    field->Accept(visitor);

                maxMessageSize += fieldMaxMessageSize + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitBytes(const EchoFieldBytes& field) override
            {
                maxMessageSize += field.maxBytesSize + MaxVarIntSize(field.maxBytesSize) + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitUint32(const EchoFieldUint32& field) override
            {
                maxMessageSize += MaxVarIntSize(std::numeric_limits<uint32_t>::max()) + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitEnum(const EchoFieldEnum& field) override
            {
                maxMessageSize += MaxVarIntSize(std::numeric_limits<uint32_t>::max()) + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitSFixed64(const EchoFieldSFixed64& field) override
            {
                maxMessageSize += 8 + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitSFixed32(const EchoFieldSFixed32& field) override
            {
                maxMessageSize += 4 + MaxVarIntSize((field.number << 3) | 2);
            }

            virtual void VisitRepeated(const EchoFieldRepeated& field) override
            {
                class GenerateNestedMaxMessageSizeVisitor
                    : public EchoFieldVisitor
                {
                public:
                    GenerateNestedMaxMessageSizeVisitor(uint32_t maxArraySize, uint32_t& maxMessageSize)
                        : maxArraySize(maxArraySize)
                        , maxMessageSize(maxMessageSize)
                    {}

                    virtual void VisitInt64(const EchoFieldInt64& field) override
                    {
                    }

                    virtual void VisitUint64(const EchoFieldUint64& field) override
                    {
                    }

                    virtual void VisitInt32(const EchoFieldInt32& field) override
                    {
                    }

                    virtual void VisitFixed64(const EchoFieldFixed64& field) override
                    {
                    }

                    virtual void VisitFixed32(const EchoFieldFixed32& field) override
                    {
                    }

                    virtual void VisitBool(const EchoFieldBool& field) override
                    {
                    }

                    virtual void VisitString(const EchoFieldString& field) override
                    {
                        maxMessageSize += maxArraySize * (field.maxStringSize + MaxVarIntSize(field.maxStringSize) + MaxVarIntSize((field.number << 3) | 2));
                    }

                    virtual void VisitStdString(const EchoFieldStdString& field) override
                    {
                    }

                    virtual void VisitMessage(const EchoFieldMessage& field) override
                    {
                        uint32_t fieldMaxMessageSize = 0;
                        GenerateMaxMessageSizeVisitor visitor(fieldMaxMessageSize);
                        for (auto& field : field.message->fields)
                            field->Accept(visitor);

                        maxMessageSize += maxArraySize * (fieldMaxMessageSize + MaxVarIntSize((field.number << 3) | 2));
                    }

                    virtual void VisitBytes(const EchoFieldBytes& field) override
                    {
                    }

                    virtual void VisitUint32(const EchoFieldUint32& field) override
                    {
                        maxMessageSize += maxArraySize * (MaxVarIntSize(std::numeric_limits<uint32_t>::max()) + MaxVarIntSize((field.number << 3) | 2));
                    }

                    virtual void VisitEnum(const EchoFieldEnum& field) override
                    {
                    }

                    virtual void VisitSFixed64(const EchoFieldSFixed64& field) override
                    {
                    }

                    virtual void VisitSFixed32(const EchoFieldSFixed32& field) override
                    {
                    }

                    virtual void VisitRepeated(const EchoFieldRepeated& field) override
                    {
                        std::abort();
                    }

                private:
                    uint32_t maxArraySize;
                    uint32_t& maxMessageSize;
                };

                uint32_t max = 0;
                GenerateNestedMaxMessageSizeVisitor visitor(field.maxArraySize, max);
                field.type->Accept(visitor);

                maxMessageSize = max;
            }

        private:
            uint32_t& maxMessageSize;
        };

        try
        {
            uint32_t max = 0;
            GenerateMaxMessageSizeVisitor visitor(max);
            for (auto& field : fields)
                field->Accept(visitor);

            maxMessageSize = max;
        }
        catch (NoMaxMessageSize&)   //NOSONAR
        {}
    }

    void EchoFieldInt32::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitInt32(*this);
    }

    void EchoFieldInt64::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitInt64(*this);
    }

    void EchoFieldFixed32::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitFixed32(*this);
    }

    void EchoFieldFixed64::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitFixed64(*this);
    }

    void EchoFieldBool::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitBool(*this);
    }

    EchoFieldString::EchoFieldString(const google::protobuf::FieldDescriptor& descriptor)
        : EchoField(descriptor)
        , maxStringSize(descriptor.options().GetExtension(string_size))
    {
        assert(maxStringSize != 0);
    }

    void EchoFieldString::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitString(*this);
    }

    EchoFieldStdString::EchoFieldStdString(const google::protobuf::FieldDescriptor& descriptor)
        : EchoField(descriptor)
    {}

    void EchoFieldStdString::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitStdString(*this);
    }

    EchoFieldMessage::EchoFieldMessage(const google::protobuf::FieldDescriptor& descriptor, EchoRoot& root)
        : EchoField(descriptor)
        , message(root.GetMessage(*descriptor.message_type()))
        , descriptor(descriptor)
    {}

    void EchoFieldMessage::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitMessage(*this);
    }

    EchoFieldBytes::EchoFieldBytes(const google::protobuf::FieldDescriptor& descriptor)
        : EchoField(descriptor)
        , maxBytesSize(descriptor.options().GetExtension(bytes_size))
    {
        if (maxBytesSize == 0)
            throw UnspecifiedBytesSize{ name };
    }

    void EchoFieldBytes::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitBytes(*this);
    }

    void EchoFieldUint32::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitUint32(*this);
    }

    void EchoFieldUint64::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitUint64(*this);
    }

    EchoFieldEnum::EchoFieldEnum(const google::protobuf::FieldDescriptor& descriptor, EchoRoot& root)
        : EchoField(descriptor)
        , type(root.GetEnum(*descriptor.enum_type()))
    {}

    void EchoFieldEnum::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitEnum(*this);
    }

    void EchoFieldSFixed32::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitSFixed32(*this);
    }

    void EchoFieldSFixed64::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitSFixed64(*this);
    }

    EchoFieldRepeated::EchoFieldRepeated(const google::protobuf::FieldDescriptor& descriptor, const std::shared_ptr<EchoField>& type)
        : EchoField(descriptor)
        , maxArraySize(descriptor.options().GetExtension(array_size))
        , type(type)
    {
        if (maxArraySize == 0)
            throw UnspecifiedArraySize{ name };
    }

    void EchoFieldRepeated::Accept(EchoFieldVisitor& visitor) const
    {
        visitor.VisitRepeated(*this);
    }

    EchoMethod::EchoMethod(const google::protobuf::MethodDescriptor& descriptor, EchoRoot& root)
        : name(descriptor.name())
        , methodId(descriptor.options().GetExtension(method_id))
    {
        if (methodId == 0)
            throw UnspecifiedMethodId{ "", name };

        if (descriptor.input_type()->full_name() != Nothing::descriptor()->full_name())
            parameter = root.GetMessage(*descriptor.input_type());
    }

    EchoService::EchoService(const google::protobuf::ServiceDescriptor& descriptor, EchoRoot& root)
        : descriptor(descriptor)
        , name(descriptor.name())
        , serviceId(descriptor.options().GetExtension(service_id))
    {
        if (serviceId == 0)
            throw UnspecifiedServiceId{ name };

        try
        {
            for (int i = 0; i != descriptor.method_count(); ++i)
                methods.emplace_back(*descriptor.method(i), root);
        }
        catch (UnspecifiedMethodId& exception)
        {
            throw UnspecifiedMethodId{ name, exception.method };
        }
    }

    EchoFile::EchoFile(const google::protobuf::FileDescriptor& file, EchoRoot& root)
    {
        name = google::protobuf::compiler::cpp::StripProto(file.name());
        packageParts = google::protobuf::Split(file.package(), ".", true);

        for (int i = 0; i != file.dependency_count(); ++i)
            if (file.dependency(i)->name() != "EchoAttributes.proto")
                dependencies.push_back(root.GetFile(*file.dependency(i)));

        for (int i = 0; i != file.enum_type_count(); ++i)
            enums.push_back(root.AddEnum(*file.enum_type(i)));

        for (int i = 0; i != file.message_type_count(); ++i)
            messages.push_back(root.AddMessage(*file.message_type(i)));

        for (int i = 0; i != file.service_count(); ++i)
            services.push_back(root.AddService(*file.service(i)));
    }

    EchoRoot::EchoRoot(const google::protobuf::FileDescriptor& rootFile)
    {
        GetFile(rootFile);
    }

    void EchoRoot::AddDescriptorSet(const google::protobuf::FileDescriptorSet& descriptorSet)
    {
        std::vector<const google::protobuf::FileDescriptor*> fileDescriptors;

        for (int i = 0; i != descriptorSet.file_size(); ++i)
        {
            auto file = pool.BuildFile(descriptorSet.file(i));
            assert(file != NULL);
            fileDescriptors.push_back(file);
        }

        GetFile(*fileDescriptors.back());
    }

    std::shared_ptr<EchoFile> EchoRoot::GetFile(const google::protobuf::FileDescriptor& fileDescriptor)
    {
        for (auto file : files)
            if (file->name == google::protobuf::compiler::cpp::StripProto(fileDescriptor.name()))
                return file;

        auto result = std::make_shared<EchoFile>(fileDescriptor, *this);
        files.push_back(result);
        return result;
    }

    std::shared_ptr<EchoMessage> EchoRoot::AddMessage(const google::protobuf::Descriptor& descriptor)
    {
        for (auto message : messages)
            if (&message->descriptor == &descriptor)
                std::abort();

        auto result = std::make_shared<EchoMessage>(descriptor, *this);
        messages.push_back(result);
        return result;
    }

    std::shared_ptr<EchoMessage> EchoRoot::GetMessage(const google::protobuf::Descriptor& descriptor)
    {
        for (auto message : messages)
            if (&message->descriptor == &descriptor)
                return message;

        throw MessageNotFound{ descriptor.name() };
    }

    std::shared_ptr<EchoEnum> EchoRoot::AddEnum(const google::protobuf::EnumDescriptor& descriptor)
    {
        for (auto enum_ : enums)
            if (&enum_->descriptor == &descriptor)
                std::abort();

        auto result = std::make_shared<EchoEnum>(descriptor);
        enums.push_back(result);
        return result;
    }

    std::shared_ptr<EchoEnum> EchoRoot::GetEnum(const google::protobuf::EnumDescriptor& descriptor)
    {
        for (auto enum_ : enums)
            if (&enum_->descriptor == &descriptor)
                return enum_;

        throw EnumNotFound{ descriptor.name() };
    }

    std::shared_ptr<EchoService> EchoRoot::AddService(const google::protobuf::ServiceDescriptor& descriptor)
    {
        for (auto service : services)
            if (&service->descriptor == &descriptor)
                std::abort();

        auto result = std::make_shared<EchoService>(descriptor, *this);
        services.push_back(result);
        return result;
    }
}
