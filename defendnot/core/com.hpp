#pragma once
#include <format>
#include <source_location>
#include <stdexcept>
#include <thread>

#include "core/log.hpp"
#include "shared/com.hpp"
#include "shared/strings.hpp"

#include <Windows.h>

namespace defendnot {
    namespace detail {
        constexpr GUID CLSID_WscIsv = {0xF2102C37, 0x90C3, 0x450C, {0xB3, 0x0F6, 0x92, 0xBE, 0x16, 0x93, 0xBD, 0xF2}};

        constexpr GUID IID_IWscFWStatus = {0x9B8F6C6E, 0x8A4A, 0x4891, {0xAF, 0x63, 0x1A, 0x2F, 0x50, 0x92, 0x40, 0x40}};
        constexpr GUID IID_IWscFWStatus2 = {0x62F698CB, 0x94A, 0x4C68, {0x94, 0x19, 0x8E, 0x8C, 0x49, 0x42, 0x0E, 0x59}};

        constexpr GUID IID_IWscAVStatus = {0x3901A765, 0xAB91, 0x4BA9, {0xA5, 0x53, 0x5B, 0x85, 0x38, 0xDE, 0xB8, 0x40}};
        constexpr GUID IID_IWscAVStatus3 = {0xCF007CA2, 0xF5E3, 0x11E5, {0x9C, 0xE9, 0x5E, 0x55, 0x17, 0x50, 0x7C, 0x66}};
        constexpr GUID IID_IWscAVStatus4 = {0x4DCBAFAC, 0x29BA, 0x46B1, {0x80, 0xFC, 0xB8, 0xBD, 0xE3, 0xC0, 0xAE, 0x4D}};

        constexpr GUID IID_IWscASStatus = {0x24E9756, 0xBA6C, 0x4AD1, {0x83, 0x21, 0x87, 0xBA, 0xE7, 0x8F, 0xD0, 0xE3}};
    } // namespace detail

    enum class WSCSecurityProductState : std::uint32_t {
        ON = 0,
        OFF = 1,
        SNOOZED = 2,
        EXPIRED = 3
    };

    enum class WSCSecurityProductSubStatus : std::uint32_t {
        NOT_SET = 0,
        NO_ACTION = 1,
        ACTION_RECOMMENDED = 2,
        ACTION_NEEDED = 3,
    };

    class IWscAVStatus4 : public com::IBaseObject<detail::CLSID_WscIsv, detail::IID_IWscAVStatus4> {
    public:
        virtual HRESULT COM_CALLCONV Register(BSTR path_to_signed_product_exe, BSTR display_name, std::uint32_t, std::uint32_t) = 0;
        virtual HRESULT COM_CALLCONV Unregister() = 0;
        virtual HRESULT COM_CALLCONV UpdateStatus(WSCSecurityProductState state, BOOL unk) = 0;
        virtual HRESULT COM_CALLCONV InitiateOfflineCleaning(std::uint16_t*, std::uint16_t*) = 0;
        virtual HRESULT COM_CALLCONV NotifyUserForNearExpiration(std::uint32_t) = 0;
        virtual HRESULT COM_CALLCONV MakeDefaultProductRequest() = 0;
        virtual HRESULT COM_CALLCONV IsDefaultProductEnforced(std::uint32_t* result) = 0;
        virtual HRESULT COM_CALLCONV UpdateScanSubstatus(WSCSecurityProductSubStatus status) = 0;
        virtual HRESULT COM_CALLCONV UpdateSettingsSubstatus(WSCSecurityProductSubStatus status) = 0;
        virtual HRESULT COM_CALLCONV UpdateProtectionUpdateSubstatus(WSCSecurityProductSubStatus status) = 0;
        virtual HRESULT COM_CALLCONV RegisterAV(std::uint16_t*, std::uint16_t*, std::uint32_t, std::uint32_t) = 0;
        virtual HRESULT COM_CALLCONV UnregisterAV() = 0;
        virtual HRESULT COM_CALLCONV UpdateStatusAV(WSCSecurityProductState state, BOOL unk) = 0;
        virtual HRESULT COM_CALLCONV InitiateOfflineCleaningAV(std::uint16_t*, std::uint16_t*) = 0;
        virtual HRESULT COM_CALLCONV NotifyUserForNearExpirationAV(std::uint32_t) = 0;
        virtual HRESULT COM_CALLCONV RegisterFW(std::uint16_t*, std::uint16_t*, std::uint32_t, std::uint32_t) = 0;
        virtual HRESULT COM_CALLCONV UnregisterFW() = 0;
        virtual HRESULT COM_CALLCONV UpdateStatusFW(WSCSecurityProductState state) = 0;
        virtual HRESULT COM_CALLCONV RegisterAS(std::uint16_t*, std::uint16_t*, std::uint32_t, std::uint32_t) = 0;
        virtual HRESULT COM_CALLCONV UnregisterAS() = 0;
        virtual HRESULT COM_CALLCONV UpdateStatusAS(WSCSecurityProductState state, BOOL unk) = 0;
    };

    class IWscASStatus : public com::IBaseObject<detail::CLSID_WscIsv, detail::IID_IWscASStatus> {
    public:
        virtual HRESULT COM_CALLCONV Register(BSTR path_to_signed_product_exe, BSTR display_name, std::uint32_t, std::uint32_t) = 0;
        virtual HRESULT COM_CALLCONV Unregister() = 0;
        virtual HRESULT COM_CALLCONV UpdateStatus(WSCSecurityProductState state, BOOL unk) = 0;
    };
} // namespace defendnot
