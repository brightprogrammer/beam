#ifndef BEAM_FRONTEND_H
#define BEAM_FRONTEND_H

#include <beam/html.h>
#include <beam/file.h>

HtmlComponent* RenderFileContent(Html* html, const char* filepath);
HtmlComponent* Render404(Html* html, void*);
HtmlComponent* RenderDirEntryInTable(Html* html, DirEntry* entry);
HtmlComponent* RenderDirContents(Html* html, DirContents* dir_contents);
HtmlComponent* RenderContent(Html* html, void*);
HtmlComponent* RenderBase(Html* html, void*);

#endif // BEAM_FRONTEND_H
