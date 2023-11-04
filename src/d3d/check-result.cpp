#include <sstream>
#include "check-result.hpp"

void checkResult(HRESULT r, const char *name) {
    if (FAILED(r)) {
        std::ostringstream errorMsg;
        errorMsg
                << "Function "
                << name
                << " failed with HRESULT of "
                << std::hex << std::uppercase << r << std::nouppercase
                << ": "
                << _com_error(r).ErrorMessage()
                << std::endl;
        OutputDebugString(errorMsg.str().c_str());
    }
}