#pragma once

#include "RareClassValidation.generated.h"

class IWidgetCompilerLog;
class UUserWidget;

/**
 * Utility class to validate UUserWidget classes for properties that are not populated in widgets (i.e. missing references).
 * Supports only generic properties that the UI is using. If a property type is not supported, feel free to add it or override the ValidateCompiledDefaults yourself in the project.
 */
UCLASS(ClassGroup=UI)
class RAREUI_API URareClassValidation : public UObject
{
	GENERATED_BODY()
	
#if WITH_EDITOR
public:
	static void ValidateCompiledDefaultsForClass(const UUserWidget* InWidgetToValidate, IWidgetCompilerLog& InCompileLog);

private:
	static bool IsPropertyValid(const FProperty* Property, const UUserWidget* Widget);
	
	static bool IsPropertyValid_SoftObject(const FSoftObjectProperty* Property, const UUserWidget* Widget);
	static bool IsPropertyValid_SoftClass(const FSoftClassProperty* Property, const UUserWidget* Widget);
	static bool IsPropertyValid_SubClass(const FClassProperty* Property, const UUserWidget* Widget);
	static bool IsPropertyValid_Object(const FObjectPropertyBase* Property, const UUserWidget* Widget);
	static bool IsPropertyValid_Struct(const FStructProperty* Property, const UUserWidget* Widget);
	static bool IsPropertyValid_Enum(const FEnumProperty* Property, const UUserWidget* Widget);

	static void ReportValidationError(IWidgetCompilerLog& CompileLog, const UUserWidget* Widget, const FProperty* Property);
#endif // WITH_EDITOR
};
