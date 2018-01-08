/*
 **
 ** Copyright 2016, The Android Open Source Project
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 */

#ifndef SYSTEM_VOLD_KEYSTORE_HIDL_SUPPORT_H_
#define SYSTEM_VOLD_KEYSTORE_HIDL_SUPPORT_H_

#include <ostream>
#include <sstream>
#include <string>

#include <hidl/Status.h>

#include "keymaster_tags.h"

namespace keystore {

inline static std::ostream& formatArgs(std::ostream& out) {
    return out;
}

template <typename First, typename... Args>
inline static std::ostream& formatArgs(std::ostream& out, First&& first, Args&&... args) {
    out << first;
    return formatArgs(out, args...);
}

template <typename... Args>
inline static std::string argsToString(Args&&... args) {
    std::stringstream s;
    formatArgs(s, args...);
    return s.str();
}

template <typename... Msgs>
inline static ErrorCode ksHandleHidlError(const Return<ErrorCode>& error, Msgs&&... msgs) {
    if (!error.isOk()) {
        ALOGE("HIDL call failed with %s @ %s", error.description().c_str(),
              argsToString(msgs...).c_str());
        return ErrorCode::UNKNOWN_ERROR;
    }
    return ErrorCode(error);
}
template <typename... Msgs>
inline static ErrorCode ksHandleHidlError(const Return<void>& error, Msgs&&... msgs) {
    if (!error.isOk()) {
        ALOGE("HIDL call failed with %s @ %s", error.description().c_str(),
              argsToString(msgs...).c_str());
        return ErrorCode::UNKNOWN_ERROR;
    }
    return ErrorCode::OK;
}

#define KS_HANDLE_HIDL_ERROR(rc) \
    ::keystore::ksHandleHidlError(rc, __FILE__, ":", __LINE__, ":", __PRETTY_FUNCTION__)

inline static hidl_vec<uint8_t> blob2hidlVec(const uint8_t* data, const size_t length,
                                             bool inPlace = true) {
    hidl_vec<uint8_t> result;
    if (inPlace)
        result.setToExternal(const_cast<unsigned char*>(data), length);
    else {
        result.resize(length);
        memcpy(&result[0], data, length);
    }
    return result;
}

inline static hidl_vec<uint8_t> blob2hidlVec(const std::string& value) {
    hidl_vec<uint8_t> result;
    result.setToExternal(
        reinterpret_cast<uint8_t*>(const_cast<std::string::value_type*>(value.data())),
        static_cast<size_t>(value.size()));
    return result;
}

inline static hidl_vec<uint8_t> blob2hidlVec(const std::vector<uint8_t>& blob) {
    hidl_vec<uint8_t> result;
    result.setToExternal(const_cast<uint8_t*>(blob.data()), static_cast<size_t>(blob.size()));
    return result;
}

template <typename T, typename OutIter>
inline static OutIter copy_bytes_to_iterator(const T& value, OutIter dest) {
    const uint8_t* value_ptr = reinterpret_cast<const uint8_t*>(&value);
    return std::copy(value_ptr, value_ptr + sizeof(value), dest);
}

inline std::string hidlVec2String(const hidl_vec<uint8_t>& value) {
    return std::string(reinterpret_cast<const std::string::value_type*>(&value[0]), value.size());
}

}  // namespace keystore

#endif  // SYSTEM_VOLD_KEYSTORE_HIDL_SUPPORT_H_
