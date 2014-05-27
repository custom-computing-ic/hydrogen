// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: packet.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "packet.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* ControlMSG_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ControlMSG_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_packet_2eproto() {
  protobuf_AddDesc_packet_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "packet.proto");
  GOOGLE_CHECK(file != NULL);
  ControlMSG_descriptor_ = file->message_type(0);
  static const int ControlMSG_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ControlMSG, command_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ControlMSG, arg1_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ControlMSG, arg2_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ControlMSG, arg3_),
  };
  ControlMSG_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ControlMSG_descriptor_,
      ControlMSG::default_instance_,
      ControlMSG_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ControlMSG, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ControlMSG, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ControlMSG));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_packet_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ControlMSG_descriptor_, &ControlMSG::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_packet_2eproto() {
  delete ControlMSG::default_instance_;
  delete ControlMSG_reflection_;
}

void protobuf_AddDesc_packet_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\014packet.proto\"G\n\nControlMSG\022\017\n\007command\030"
    "\001 \002(\t\022\014\n\004arg1\030\002 \001(\t\022\014\n\004arg2\030\003 \001(\t\022\014\n\004arg"
    "3\030\004 \001(\t", 87);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "packet.proto", &protobuf_RegisterTypes);
  ControlMSG::default_instance_ = new ControlMSG();
  ControlMSG::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_packet_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_packet_2eproto {
  StaticDescriptorInitializer_packet_2eproto() {
    protobuf_AddDesc_packet_2eproto();
  }
} static_descriptor_initializer_packet_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int ControlMSG::kCommandFieldNumber;
const int ControlMSG::kArg1FieldNumber;
const int ControlMSG::kArg2FieldNumber;
const int ControlMSG::kArg3FieldNumber;
#endif  // !_MSC_VER

ControlMSG::ControlMSG()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ControlMSG::InitAsDefaultInstance() {
}

ControlMSG::ControlMSG(const ControlMSG& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ControlMSG::SharedCtor() {
  _cached_size_ = 0;
  command_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  arg1_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  arg2_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  arg3_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ControlMSG::~ControlMSG() {
  SharedDtor();
}

void ControlMSG::SharedDtor() {
  if (command_ != &::google::protobuf::internal::kEmptyString) {
    delete command_;
  }
  if (arg1_ != &::google::protobuf::internal::kEmptyString) {
    delete arg1_;
  }
  if (arg2_ != &::google::protobuf::internal::kEmptyString) {
    delete arg2_;
  }
  if (arg3_ != &::google::protobuf::internal::kEmptyString) {
    delete arg3_;
  }
  if (this != default_instance_) {
  }
}

void ControlMSG::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ControlMSG::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ControlMSG_descriptor_;
}

const ControlMSG& ControlMSG::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_packet_2eproto();
  return *default_instance_;
}

ControlMSG* ControlMSG::default_instance_ = NULL;

ControlMSG* ControlMSG::New() const {
  return new ControlMSG;
}

void ControlMSG::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_command()) {
      if (command_ != &::google::protobuf::internal::kEmptyString) {
        command_->clear();
      }
    }
    if (has_arg1()) {
      if (arg1_ != &::google::protobuf::internal::kEmptyString) {
        arg1_->clear();
      }
    }
    if (has_arg2()) {
      if (arg2_ != &::google::protobuf::internal::kEmptyString) {
        arg2_->clear();
      }
    }
    if (has_arg3()) {
      if (arg3_ != &::google::protobuf::internal::kEmptyString) {
        arg3_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ControlMSG::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string command = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_command()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->command().data(), this->command().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_arg1;
        break;
      }

      // optional string arg1 = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_arg1:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_arg1()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->arg1().data(), this->arg1().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_arg2;
        break;
      }

      // optional string arg2 = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_arg2:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_arg2()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->arg2().data(), this->arg2().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_arg3;
        break;
      }

      // optional string arg3 = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_arg3:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_arg3()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->arg3().data(), this->arg3().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ControlMSG::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string command = 1;
  if (has_command()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->command().data(), this->command().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->command(), output);
  }

  // optional string arg1 = 2;
  if (has_arg1()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->arg1().data(), this->arg1().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->arg1(), output);
  }

  // optional string arg2 = 3;
  if (has_arg2()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->arg2().data(), this->arg2().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->arg2(), output);
  }

  // optional string arg3 = 4;
  if (has_arg3()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->arg3().data(), this->arg3().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->arg3(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ControlMSG::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string command = 1;
  if (has_command()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->command().data(), this->command().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->command(), target);
  }

  // optional string arg1 = 2;
  if (has_arg1()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->arg1().data(), this->arg1().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->arg1(), target);
  }

  // optional string arg2 = 3;
  if (has_arg2()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->arg2().data(), this->arg2().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->arg2(), target);
  }

  // optional string arg3 = 4;
  if (has_arg3()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->arg3().data(), this->arg3().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->arg3(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ControlMSG::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string command = 1;
    if (has_command()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->command());
    }

    // optional string arg1 = 2;
    if (has_arg1()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->arg1());
    }

    // optional string arg2 = 3;
    if (has_arg2()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->arg2());
    }

    // optional string arg3 = 4;
    if (has_arg3()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->arg3());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ControlMSG::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ControlMSG* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ControlMSG*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ControlMSG::MergeFrom(const ControlMSG& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_command()) {
      set_command(from.command());
    }
    if (from.has_arg1()) {
      set_arg1(from.arg1());
    }
    if (from.has_arg2()) {
      set_arg2(from.arg2());
    }
    if (from.has_arg3()) {
      set_arg3(from.arg3());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ControlMSG::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ControlMSG::CopyFrom(const ControlMSG& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ControlMSG::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void ControlMSG::Swap(ControlMSG* other) {
  if (other != this) {
    std::swap(command_, other->command_);
    std::swap(arg1_, other->arg1_);
    std::swap(arg2_, other->arg2_);
    std::swap(arg3_, other->arg3_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ControlMSG::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ControlMSG_descriptor_;
  metadata.reflection = ControlMSG_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
