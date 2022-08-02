#include "FileDialog.hpp"
#include <nfd.h>

namespace board
{
    using utils::FileOpenError;
    using utils::FileWriteError;

    const string extension = "lua";
    const string dot_extension = "." + extension;

    string FileDialog::Open()
    {
        nfdchar_t* outPath = NULL;
        nfdresult_t result = NFD_OpenDialog(extension.data(), NULL, &outPath);

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

    vector<string> FileDialog::OpenMultiple()
    {
        nfdpathset_t outPaths;
        nfdresult_t result = NFD_OpenDialogMultiple(extension.data(), NULL, &outPaths);
        vector<string> paths;

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

    string FileDialog::Save()
    {
        nfdchar_t* outPath = NULL;
        nfdresult_t result = NFD_SaveDialog(extension.data(), NULL, &outPath);

        switch (result)
        {
        case (NFD_OKAY):
        {
            string ret = outPath;
            if (!ret.contains(dot_extension))
            {
                ret += dot_extension;
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