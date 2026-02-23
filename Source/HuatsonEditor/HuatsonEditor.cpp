
#include "HuatsonEditor.h"

class SWidget;

#define LOCTEXT_NAMESPACE "HuatsonEditor"

DEFINE_LOG_CATEGORY(LogHuatsonEditor);

/**
 * FHuatsonEditorModule
 */

class FHuatsonEditorModule : public FDefaultGameModuleImpl
{
    typedef FHuatsonEditorModule ThisClass;

    virtual void StartupModule() override
    {
    }

    virtual void ShutdownModule() override
    {
    }

};

IMPLEMENT_MODULE(FHuatsonEditorModule, HuatsonEditor);

#undef LOCTEXT_NAMESPACE
