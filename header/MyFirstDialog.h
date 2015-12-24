#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include "WDL/WDL/swell/swell.h"
#endif
#include "utilfuncs.h"
#include <memory>
#include <functional>
#include <vector>
#include <unordered_set>
#include "mrpwincontrols.h"

class ReaperDialog : public NoCopyNoMove
{
public:
	ReaperDialog(HWND parent, int dialogresource, std::function<bool(HWND, UINT, WPARAM, LPARAM)> proc);
	virtual ~ReaperDialog();
	std::function<bool(HWND, UINT, WPARAM, LPARAM)> DialogProc;
	HWND getWindowHandle() const { return m_hwnd; }
	bool isVisible() const;
	void setVisible(bool b);
	void add_command_handler(WORD control, WORD id, std::function<void(void)> f);
	void add_text_changed_handler(WORD control, WORD id, std::function<void(std::string)> f);
private:
	HWND m_hwnd = NULL;
	static INT_PTR CALLBACK dlgproc(HWND, UINT, WPARAM, LPARAM);
	struct callback_entry_t
	{
		WORD m_control_id = 0;
		WORD m_notification_id = 0;
		std::function<void(void)> m_func;
		std::function<void(std::string)> m_text_changed_func;
	};
	std::vector<callback_entry_t> m_simple_command_handlers;
};

void clean_up_gui();