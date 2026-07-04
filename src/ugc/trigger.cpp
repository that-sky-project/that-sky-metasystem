#include "trigger.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Trigger
// ----------------------------------------------------------------------------

META_REGISTER_CLASS(Trigger, MetaClassImpl<Event>::Must_call_META_REGISTER_CLASS)

// ----------------------------------------------------------------------------
// [SECTION] WinMessageBoxA
// ----------------------------------------------------------------------------

META_REGISTER_CLASS(WinMessageBoxA, MetaClassImpl<Trigger>::Must_call_META_REGISTER_CLASS)
META_DATA_CLASS(WinMessageBoxA, Tool_Export, "true")

META_REGISTER_SIMPLE_MEMBER(WinMessageBoxA, lpText)
META_DATA_MEMBER_VARIABLE(WinMessageBoxA, lpText, Tool_DefaultValue, "")
META_DATA_MEMBER_VARIABLE(WinMessageBoxA, lpText, Tool_Description, "Text inside the message box.")

META_REGISTER_SIMPLE_MEMBER(WinMessageBoxA, lpCaption)
META_DATA_MEMBER_VARIABLE(WinMessageBoxA, lpCaption, Tool_DefaultValue, "MessageBoxA")
META_DATA_MEMBER_VARIABLE(WinMessageBoxA, lpCaption, Tool_Description, "Caption of the message box.")

META_REGISTER_SIMPLE_MEMBER(WinMessageBoxA, uType)
META_DATA_MEMBER_VARIABLE(WinMessageBoxA, uType, Tool_DefaultValue, "0")
META_DATA_MEMBER_VARIABLE(WinMessageBoxA, uType, Tool_Description, "Type of the MessageBox.\nDefault value is MB_OK.")

// ----------------------------------------------------------------------------
// [SECTION] FowardTrigger
// ----------------------------------------------------------------------------

META_REGISTER_CLASS(FowardTrigger, MetaClassImpl<Trigger>::Must_call_META_REGISTER_CLASS)
META_DATA_CLASS(FowardTrigger, Tool_Export, "true")

META_REGISTER_SIMPLE_MEMBER(FowardTrigger, src)
META_DATA_MEMBER_VARIABLE(FowardTrigger, src, Tool_DefaultValue, "explorer")
META_DATA_MEMBER_VARIABLE(FowardTrigger, src, Tool_Description, "The shell command to execute.")
