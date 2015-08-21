// PluginHelpers.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "pluginhelpers.h"

#include "..\Interfaces\ITasklist.h"
#include "..\Interfaces\ITransText.h"
#include "..\Interfaces\IPreferences.h"

using namespace PluginHelpers;
using namespace System;
using namespace System::Runtime::InteropServices;

////////////////////////////////////////////////////////////////////////////////////////////////

// converts System::String to LPCWSTR, and frees memory on exit
class CMarshallString
{
public:
   CMarshallString(String^ str) : m_wszGlobal(NULL)
   {
      m_wszGlobal = (LPCWSTR)Marshal::StringToHGlobalUni(str).ToPointer();
   }

   ~CMarshallString()
   {
      Marshal::FreeHGlobal((IntPtr)(void*)m_wszGlobal);
   }

   operator LPCWSTR() { return m_wszGlobal; }

private:
   LPCWSTR m_wszGlobal;
};

#define MS(str) CMarshallString(str)

////////////////////////////////////////////////////////////////////////////////////////////////

TDLPreferences::TDLPreferences(IPreferences* pPrefs) : m_pPrefs(pPrefs) 
{
   int breakpoint = 0;
} 

// private constructor
TDLPreferences::TDLPreferences() : m_pPrefs(NULL) 
{

}

// ---------------------------------------------------------

#define GETPREF(fn, defval) \
   (m_pPrefs ? m_pPrefs->fn(MS(sSection), MS(sEntry), defval) : defval)

#define SETPREF(fn, val) \
   (m_pPrefs ? (m_pPrefs->fn(MS(sSection), MS(sEntry), val) != FALSE) : false)

// ---------------------------------------------------------

int TDLPreferences::GetProfileInt(String^ sSection, String^ sEntry, int nDefault)
{
   return GETPREF(GetProfileInt, nDefault);
}

bool TDLPreferences::WriteProfileInt(String^ sSection, String^ sEntry, int nValue)
{
   return SETPREF(WriteProfileInt, nValue);
}

String^ TDLPreferences::GetProfileString(String^ sSection, String^ sEntry, String^ sDefault)
{
   return gcnew String(GETPREF(GetProfileString, MS(sDefault)));
}

bool TDLPreferences::WriteProfileString(String^ sSection, String^ sEntry, String^ sValue)
{
   return SETPREF(WriteProfileString, MS(sValue));
}

double TDLPreferences::GetProfileDouble(String^ sSection, String^ sEntry, double dDefault)
{
   return GETPREF(GetProfileDouble, dDefault);
}

bool TDLPreferences::WriteProfileDouble(String^ sSection, String^ sEntry, double dValue)
{
   return SETPREF(WriteProfileDouble, dValue);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTaskList 
TDLTaskList::TDLTaskList(ITaskList14* pTaskList) : m_pTaskList(pTaskList), m_pConstTaskList(NULL) 
{
   int breakpoint = 0;
} 

TDLTaskList::TDLTaskList(const ITaskList14* pTaskList) : m_pTaskList(NULL), m_pConstTaskList(pTaskList) 
{
   int breakpoint = 0;
} 

// private constructor
TDLTaskList::TDLTaskList() : m_pTaskList(NULL), m_pConstTaskList(NULL)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////
// GETTERS

#define GETVAL(fn, errret) \
   (m_pConstTaskList ? m_pConstTaskList->fn() : (m_pTaskList ? m_pTaskList->fn() : errret))

#define GETSTR(fn) \
   gcnew String(m_pConstTaskList ? m_pConstTaskList->fn() : (m_pTaskList ? m_pTaskList->fn() : L""))

#define GETVAL_ARG(fn, arg, errret) \
   (m_pConstTaskList ? m_pConstTaskList->fn(arg) : (m_pTaskList ? m_pTaskList->fn(arg) : errret))

#define GETSTR_ARG(fn, arg) \
   gcnew String(m_pConstTaskList ? m_pConstTaskList->fn(arg) : (m_pTaskList ? m_pTaskList->fn(arg) : L""))

// -------------------------------------------------------------

String^ TDLTaskList::GetReportTitle()
{
   return GETSTR(GetReportTitle);
}

String^ TDLTaskList::GetReportDate()
{
   return GETSTR(GetReportDate);
}

String^ TDLTaskList::GetMetaData(String^ sKey)
{
   return GETSTR_ARG(GetMetaData, MS(sKey));
}

UInt32 TDLTaskList::GetCustomAttributeCount()
{
   return GETVAL(GetCustomAttributeCount, 0);
}

String^ TDLTaskList::GetCustomAttributeLabel(int nIndex)
{
   return GETSTR_ARG(GetCustomAttributeLabel, nIndex);
}

String^ TDLTaskList::GetCustomAttributeID(int nIndex)
{
   return GETSTR_ARG(GetCustomAttributeID, nIndex);
}

String^ TDLTaskList::GetCustomAttributeValue(int nIndex, String^ sItem)
{
    LPCWSTR szValue = (m_pConstTaskList ? m_pConstTaskList->GetCustomAttributeValue(nIndex, MS(sItem)) : 
                      (m_pTaskList ? m_pTaskList->GetCustomAttributeValue(nIndex, MS(sItem)) : L""));
   
    return gcnew String(szValue);
}

UInt32 TDLTaskList::GetTaskCount()
{
   return GETVAL(GetTaskCount, 0);
}

TDLTask^ TDLTaskList::FindTask(UInt32 dwTaskID)
{
   IntPtr hTask = IntPtr(GETVAL_ARG(FindTask, dwTaskID, NULL));

   return gcnew TDLTask((m_pConstTaskList ? m_pConstTaskList : m_pTaskList), hTask);
}

TDLTask^ TDLTaskList::GetFirstTask()
{
   if (m_pConstTaskList)
      return gcnew TDLTask(m_pConstTaskList, IntPtr(m_pConstTaskList->GetFirstTask(nullptr)));

   // else
   return gcnew TDLTask(m_pTaskList, IntPtr(m_pTaskList->GetFirstTask(nullptr)));
}

// ---------------------------------------------------------

#define GETTASKVAL(fn, errret) \
   (m_pConstTaskList ? m_pConstTaskList->fn(*this) : (m_pTaskList ? m_pTaskList->fn(*this) : errret))

#define GETTASKDATE(fn, errret) \
   (m_pConstTaskList ? m_pConstTaskList->fn(*this, date) : (m_pTaskList ? m_pTaskList->fn(*this, date) : errret))

#define GETTASKSTR(fn) \
   gcnew String(GETTASKVAL(fn, L""))

#define GETTASKVAL_ARG(fn, arg, errret) \
   (m_pConstTaskList ? m_pConstTaskList->fn(*this, arg) : (m_pTaskList ? m_pTaskList->fn(*this, arg) : errret))

#define GETTASKSTR_ARG(fn, arg) \
   gcnew String(GETTASKVAL_ARG(fn, arg, L""))

#define GETTASKDATE_ARG(fn, arg, errret) \
   (m_pConstTaskList ? m_pConstTaskList->fn(*this, arg, date) : (m_pTaskList ? m_pTaskList->fn(*this, arg, date) : errret))

// ---------------------------------------------------------

TDLTask::TDLTask()
   : m_pTaskList(nullptr), m_pConstTaskList(nullptr), m_hTask(IntPtr::Zero)
{

}

TDLTask::TDLTask(ITaskList14* pTaskList, IntPtr hTask) 
   : m_pTaskList(pTaskList), m_pConstTaskList(nullptr), m_hTask(hTask)
{

}

TDLTask::TDLTask(const ITaskList14* pTaskList, IntPtr hTask)
   : m_pTaskList(nullptr), m_pConstTaskList(pTaskList), m_hTask(hTask)
{

}

TDLTask::TDLTask(const TDLTask^ task)
{
   m_pTaskList = task->m_pTaskList;
   m_pConstTaskList = task->m_pConstTaskList;
   m_hTask = task->m_hTask;
}

TDLTask::TDLTask(TDLTask^ task)
{
   m_pTaskList = task->m_pTaskList;
   m_pConstTaskList = task->m_pConstTaskList;
   m_hTask = task->m_hTask;
}

bool TDLTask::IsValid()
{
   return ((m_pConstTaskList || m_pTaskList) && (m_hTask != IntPtr::Zero));
}

TDLTask::operator HTASKITEM()
{
   return (HTASKITEM)m_hTask.ToPointer();
}

TDLTask^ TDLTask::GetFirstSubtask()
{
   if (m_pConstTaskList)
      return gcnew TDLTask(m_pConstTaskList, IntPtr(m_pConstTaskList->GetFirstTask(*this)));
   
   // else
   return gcnew TDLTask(m_pTaskList, IntPtr(m_pTaskList->GetFirstTask(*this)));
}

TDLTask^ TDLTask::GetNextTask()
{
   if (m_pConstTaskList)
      return gcnew TDLTask(m_pConstTaskList, IntPtr(m_pConstTaskList->GetNextTask(*this)));

   // else
   return gcnew TDLTask(m_pTaskList, IntPtr(m_pTaskList->GetNextTask(*this)));
}

TDLTask^ TDLTask::GetParentTask()
{
   if (m_pConstTaskList)
      return gcnew TDLTask(m_pConstTaskList, IntPtr(m_pConstTaskList->GetTaskParent(*this)));

   // else
   return gcnew TDLTask(m_pTaskList, IntPtr(m_pTaskList->GetTaskParent(*this)));
}

String^ TDLTask::GetTitle()
{
   return GETTASKSTR(GetTaskTitle);
}

String^ TDLTask::GetComments()
{
   return GETTASKSTR(GetTaskComments);
}

String^ TDLTask::GetAllocatedBy()
{
   return GETTASKSTR(GetTaskAllocatedBy);
}

String^ TDLTask::GetStatus()
{
   return GETTASKSTR(GetTaskStatus);
}

String^ TDLTask::GetWebColor()
{
   return GETTASKSTR(GetTaskWebColor);
}

String^ TDLTask::GetPriorityWebColor()
{
   return GETTASKSTR(GetTaskPriorityWebColor);
}

String^ TDLTask::GetVersion()
{
   return GETTASKSTR(GetTaskVersion);
}

String^ TDLTask::GetExternalID()
{
   return GETTASKSTR(GetTaskExternalID);
}

String^ TDLTask::GetCreatedBy()
{
   return GETTASKSTR(GetTaskCreatedBy);
}

String^ TDLTask::GetPositionString()
{
   return GETTASKSTR(GetTaskPositionString);
}

String^ TDLTask::GetIcon()
{
   return GETTASKSTR(GetTaskIcon);
}

UInt32 TDLTask::GetID()
{
   return GETTASKVAL(GetTaskID, 0);
}

UInt32 TDLTask::GetColor()
{
   return GETTASKVAL(GetTaskColor, 0);
}

UInt32 TDLTask::GetTextColor()
{
   return GETTASKVAL(GetTaskTextColor, 0);
}

UInt32 TDLTask::GetPriorityColor()
{
   return GETTASKVAL(GetTaskPriorityColor, 0);
}

UInt32 TDLTask::GetPosition()
{
   return GETTASKVAL(GetTaskPosition, 0);
}

UInt32 TDLTask::GetPriority()
{
   return GETTASKVAL_ARG(GetTaskPriority, FALSE, 0);
}

UInt32 TDLTask::GetRisk()
{
   return GETTASKVAL_ARG(GetTaskRisk, FALSE, 0);
}

UInt32 TDLTask::GetCategoryCount()
{
   return GETTASKVAL(GetTaskCategoryCount, 0);
}

UInt32 TDLTask::GetAllocatedToCount()
{
   return GETTASKVAL(GetTaskAllocatedToCount, 0);
}

UInt32 TDLTask::GetTagCount()
{
   return GETTASKVAL(GetTaskTagCount, 0);
}

UInt32 TDLTask::GetDependencyCount()
{
   return GETTASKVAL(GetTaskDependencyCount, 0);
}

UInt32 TDLTask::GetFileReferenceCount()
{
   return GETTASKVAL(GetTaskFileReferenceCount, 0);
}

String^ TDLTask::GetAllocatedTo(int nIndex)
{
   return GETTASKSTR_ARG(GetTaskAllocatedTo, nIndex);
}

String^ TDLTask::GetCategory(int nIndex)
{
   return GETTASKSTR_ARG(GetTaskCategory, nIndex);
}

String^ TDLTask::GetTag(int nIndex)
{
   return GETTASKSTR_ARG(GetTaskTag, nIndex);
}

String^ TDLTask::GetDependency(int nIndex)
{
   return GETTASKSTR_ARG(GetTaskDependency, nIndex);
}

String^ TDLTask::GetFileReference(int nIndex)
{
   return GETTASKSTR_ARG(GetTaskFileReference, nIndex);
}

Byte TDLTask::GetPercentDone()
{
   return GETTASKVAL_ARG(GetTaskPercentDone, FALSE, 0);
}

double TDLTask::GetCost()
{
   return GETTASKVAL_ARG(GetTaskCost, FALSE, 0);
}

Int64 TDLTask::GetLastModified()
{
   __int64 date = 0;
   GETTASKDATE(GetTaskLastModified64, 0);

   return date;
}

Int64 TDLTask::GetDoneDate()
{
   __int64 date = 0;
   GETTASKDATE(GetTaskDoneDate64, 0);

   return date;
}

Int64 TDLTask::GetDueDate()
{
   __int64 date = 0;
   GETTASKDATE_ARG(GetTaskDueDate64, FALSE, 0);

   return date;
}

Int64 TDLTask::GetStartDate()
{
   __int64 date = 0;
   GETTASKDATE_ARG(GetTaskStartDate64, FALSE, 0);

   return date;
}

Int64 TDLTask::GetCreationDate()
{
   __int64 date = 0;
   GETTASKDATE(GetTaskCreationDate64, 0);

   return date;
}

String^ TDLTask::GetDoneDateString()
{
   return GETTASKSTR(GetTaskDoneDateString);
}

String^ TDLTask::GetDueDateString()
{
   return GETTASKSTR_ARG(GetTaskDueDateString, FALSE);
}

String^ TDLTask::GetStartDateString()
{
   return GETTASKSTR_ARG(GetTaskStartDateString, FALSE);
}

String^ TDLTask::GetCreationDateString()
{
   return GETTASKSTR(GetTaskCreationDateString);
}

Boolean TDLTask::IsDone()
{
   return GETTASKVAL(IsTaskDone, false);
}

Boolean TDLTask::IsDue()
{
   return GETTASKVAL(IsTaskDue, false);
}

Boolean TDLTask::IsGoodAsDone()
{
   return GETTASKVAL(IsTaskGoodAsDone, false);
}

Boolean TDLTask::IsFlagged()
{
   return GETTASKVAL(IsTaskFlagged, false);
}

// ---------------------------------------------------------

double TDLTask::GetTimeEstimate(Char% cUnits)
{
   WCHAR nUnits = 0;

   double dTime = (m_pConstTaskList ? m_pConstTaskList->GetTaskTimeEstimate(*this, nUnits, FALSE) :
                  (m_pTaskList ? m_pTaskList->GetTaskTimeEstimate(*this, nUnits, FALSE) : 0.0));

   cUnits = nUnits;
   return dTime;
}

double TDLTask::GetTimeSpent(Char% cUnits)
{
   WCHAR nUnits = 0;

   double dTime = (m_pConstTaskList ? m_pConstTaskList->GetTaskTimeSpent(*this, nUnits, FALSE) :
                  (m_pTaskList ? m_pTaskList->GetTaskTimeSpent(*this, nUnits, FALSE) : 0.0));

   cUnits = nUnits;
   return dTime;
}

Boolean TDLTask::GetRecurrence()
{
	// TODO
	return false;
}

Boolean TDLTask::HasAttribute(String^ sAttrib)
{
   return (m_pConstTaskList ? m_pConstTaskList->TaskHasAttribute(*this, MS(sAttrib)) : 
          (m_pTaskList ? m_pTaskList->TaskHasAttribute(*this, MS(sAttrib)) : false));
}

String^ TDLTask::GetAttribute(String^ sAttrib)
{
   LPCWSTR szValue = (m_pConstTaskList ? m_pConstTaskList->GetTaskAttribute(*this, MS(sAttrib)) : 
                     (m_pTaskList ? m_pTaskList->GetTaskAttribute(*this, MS(sAttrib)) : L""));

   return gcnew String(szValue);
}

String^ TDLTask::GetCustomAttributeData(String^ sID)
{
   LPCWSTR szValue = (m_pConstTaskList ? m_pConstTaskList->GetTaskCustomAttributeData(*this, MS(sID)) : 
                     (m_pTaskList ? m_pTaskList->GetTaskCustomAttributeData(*this, MS(sID)) : L""));

   return gcnew String(szValue);
}

String^ TDLTask::GetMetaData(String^ sKey)
{
   LPCWSTR szValue = (m_pConstTaskList ? m_pConstTaskList->GetTaskMetaData(*this, MS(sKey)) : 
                     (m_pTaskList ? m_pTaskList->GetTaskMetaData(*this, MS(sKey)) : L""));

   return gcnew String(szValue);
}

// TODO

////////////////////////////////////////////////////////////////////////////////////////////////
// SETTERS

TDLTask^ TDLTaskList::NewTask(String^ sTitle)
{
   IntPtr hTask = IntPtr(m_pTaskList ? m_pTaskList->NewTask(MS(sTitle), nullptr, 0) : nullptr);

   return gcnew TDLTask(m_pTaskList, hTask);
}

Boolean TDLTaskList::AddCustomAttribute(String^ sID, String^ sLabel)
{
   return (m_pTaskList ? m_pTaskList->AddCustomAttribute(MS(sID), MS(sLabel)) : false);
}

Boolean TDLTaskList::SetMetaData(String^ sKey, String^ sValue)
{
   return (m_pTaskList ? m_pTaskList->SetMetaData(MS(sKey), MS(sValue)) : false);
}

Boolean TDLTaskList::ClearMetaData(String^ sKey)
{
   return (m_pTaskList ? m_pTaskList->ClearMetaData(MS(sKey)) : false);
}

// ---------------------------------------------------------

#define SETTASKVAL(fn, val) \
   (m_pTaskList ? m_pTaskList->fn(*this, val) : false)

#define SETTASKSTR(fn, str) \
   (m_pTaskList ? m_pTaskList->fn(*this, MS(str)) : false)

// ---------------------------------------------------------

TDLTask^ TDLTask::NewSubtask(String^ sTitle)
{
   IntPtr hTask = IntPtr(m_pTaskList ? m_pTaskList->NewTask(MS(sTitle), nullptr, 0) : nullptr);

   return gcnew TDLTask(m_pTaskList, hTask);
}

Boolean TDLTask::SetTitle(String^ sTitle)
{
   return SETTASKSTR(SetTaskTitle, sTitle);
}

Boolean TDLTask::SetComments(String^ sComments)
{
   return SETTASKSTR(SetTaskComments, sComments);
}

Boolean TDLTask::SetAllocatedBy(String^ sAllocBy)
{
   return SETTASKSTR(SetTaskAllocatedBy, sAllocBy);
}

Boolean TDLTask::SetStatus(String^ sStatus)
{
   return SETTASKSTR(SetTaskStatus, sStatus);
}

Boolean TDLTask::SetVersion(String^ sVersion)
{
   return SETTASKSTR(SetTaskVersion, sVersion);
}

Boolean TDLTask::SetExternalID(String^ sExternalID)
{
   return SETTASKSTR(SetTaskExternalID, sExternalID);
}

Boolean TDLTask::SetCreatedBy(String^ sCreatedBy)
{
   return SETTASKSTR(SetTaskCreatedBy, sCreatedBy);
}

Boolean TDLTask::SetPosition(String^ sPosition)
{
   return SETTASKSTR(SetTaskPosition, sPosition);
}

Boolean TDLTask::SetIcon(String^ sIcon)
{
   return SETTASKSTR(SetTaskIcon, sIcon);
}

Boolean TDLTask::AddAllocatedTo(String^ sAllocTo)
{
   return SETTASKSTR(AddTaskAllocatedTo, sAllocTo);
}

Boolean TDLTask::AddCategory(String^ sCategory)
{
   return SETTASKSTR(AddTaskCategory, sCategory);
}

Boolean TDLTask::AddTag(String^ sTag)
{
   return SETTASKSTR(AddTaskTag, sTag);
}

Boolean TDLTask::AddDependency(String^ sDependency)
{
   return SETTASKSTR(AddTaskDependency, sDependency);
}

Boolean TDLTask::AddFileReference(String^ sFileLink)
{
   return SETTASKSTR(AddTaskFileReference, sFileLink);
}

Boolean TDLTask::SetColor(UINT32 color)
{
   return SETTASKVAL(SetTaskColor, color);
}

Boolean TDLTask::SetPriority(Byte nPriority)
{
   return SETTASKVAL(SetTaskPriority, nPriority);
}

Boolean TDLTask::SetRisk(Byte Risk)
{
   return SETTASKVAL(SetTaskRisk, Risk);
}

Boolean TDLTask::SetPercentDone(Byte nPercent)
{
   return SETTASKVAL(SetTaskPercentDone, nPercent);
}

Boolean TDLTask::SetCost(double dCost)
{
   return SETTASKVAL(SetTaskCost, dCost);
}

Boolean TDLTask::SetFlag(Boolean bFlag)
{
   return SETTASKVAL(SetTaskFlag, bFlag);
}

Boolean TDLTask::SetLastModified(Int64 dtLastMod)
{
   return SETTASKVAL(SetTaskLastModified, dtLastMod);
}

Boolean TDLTask::SetDoneDate(Int64 dtCompletion)
{
   return SETTASKVAL(SetTaskDoneDate, dtCompletion);
}

Boolean TDLTask::SetDueDate(Int64 dtDue)
{
   return SETTASKVAL(SetTaskDueDate, dtDue);
}

Boolean TDLTask::SetStartDate(Int64 dtStart)
{
   return SETTASKVAL(SetTaskStartDate, dtStart);
}

Boolean TDLTask::SetCreationDate(Int64 dtCreation)
{
   return SETTASKVAL(SetTaskCreationDate, dtCreation);
}

Boolean TDLTask::SetTimeEstimate(double dTime, Char cUnits)
{
   return (m_pTaskList ? m_pTaskList->SetTaskTimeEstimate(*this, dTime, cUnits) : false);
}

Boolean TDLTask::SetTimeSpent(double dTime, Char cUnits)
{
   return (m_pTaskList ? m_pTaskList->SetTaskTimeSpent(*this, dTime, cUnits) : false);
}

Boolean TDLTask::SetCustomAttributeData(String^ sID, String^ sValue)
{
   return (m_pTaskList ? m_pTaskList->SetTaskCustomAttributeData(*this, MS(sID), MS(sValue)) : false);
}

Boolean TDLTask::ClearCustomAttributeData(String^ sID)
{
   return (m_pTaskList ? m_pTaskList->ClearTaskCustomAttributeData(*this, MS(sID)) : false);
}

Boolean TDLTask::SetMetaData(String^ sKey, String^ sValue)
{
   return (m_pTaskList ? m_pTaskList->SetTaskMetaData(*this, MS(sKey), MS(sValue)) : false);
}

Boolean TDLTask::ClearMetaData(String^ sKey)
{
   return (m_pTaskList ? m_pTaskList->ClearTaskMetaData(*this, MS(sKey)) : false);
}

////////////////////////////////////////////////////////////////////////////////////////////////

TDLTranslate::TDLTranslate(ITransText* pTransText) : m_pTransText(pTransText) 
{
   int breakpoint = 0;
} 

// private constructor
TDLTranslate::TDLTranslate() : m_pTransText(NULL)
{

}

// TODO

////////////////////////////////////////////////////////////////////////////////////////////////