#include "FileDialog.hpp"
#include <nfd.h>

namespace board
{
    using utils::FileOpenError;
    using utils::FileWriteError;

    std::string FileDialog::Open()
    {
        nfdchar_t* outPath = NULL;
        nfdresult_t result = NFD_OpenDialog("lua", NULL, &outPath);

        switch (result)
        {
        case (NFD_OKAY):
            return outPath;
        case (NFD_CANCEL):
            return "";
        default:
            throw FileOpenError(NFD_GetError());
        }
    }

    std::vector<std::string> FileDialog::OpenMultiple()
    {
        nfdpathset_t outPaths;
        nfdresult_t result = NFD_OpenDialogMultiple("lua", NULL, &outPaths);
        std::vector<std::string> paths;

        switch (result)
        {
        case (NFD_OKAY):
            for (std::size_t i = 0; i < NFD_PathSet_GetCount(&outPaths); i++)
            {
                paths.emplace_back(NFD_PathSet_GetPath(&outPaths, i));
            }
            NFD_PathSet_Free(&outPaths);
            return paths;
        case (NFD_CANCEL):
            return paths;
        default:
            throw FileOpenError(NFD_GetError());
        }
    }

    std::string FileDialog::Save()
    {
        nfdchar_t* outPath = NULL;
        nfdresult_t result = NFD_SaveDialog("lua", NULL, &outPath);

        switch (result)
        {
        case (NFD_OKAY):
        {
            std::string ret = outPath;
            if (!ret.contains(".lua"))
            {
                ret += ".lua";
            }
            return ret;
        }
        case (NFD_CANCEL):
            return "";
        default:
            throw FileOpenError(NFD_GetError());
        }
    }
}