#ifndef BEAM_FRONTEND_H
#define BEAM_FRONTEND_H

#include <beam/html.h>
#include <beam/file.h>

Html* WrapFileContent(Html* html, const char* filepath);
Html* Wrap404(Html* html);
Html* WrapDirEntryInTable(Html* html, DirEntry* entry);
Html* WrapDirContents(Html* html, DirContents* dir_contents);
Html* WrapContent(Html* html);
Html* WrapBase(Html* html);

#endif // BEAM_FRONTEND_H
