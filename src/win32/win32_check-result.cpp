#include <sstream>
#include "win32_check-result.hpp"
#include "win32_kill-program.hpp"

void win32_checkResult(const HRESULT r, const char *name) {
    if (FAILED(r)) {
        std::wostringstream errorMsg;
        errorMsg
                << "Function "
                << name
                << " failed with HRESULT of "
                << std::hex << std::uppercase << r << std::nouppercase
                << ": "
                << _com_error(r).ErrorMessage()
                << std::endl;
        win32_killProgram(errorMsg.str().c_str());
    }
}
