#include "EazyClassValidation.h"

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "EazyInputAction.h"
#include "EazyUIActionData.h"
#include "Editor/WidgetCompilerLog.h"
#include "Engine/DataTable.h"

#if WITH_EDITOR
void UEazyClassValidation::ValidateCompiledDefaultsForClass(const UUserWidget* InWidgetToValidate, IWidgetCompilerLog& InCompileLog)
{
	if (!IsValid(InWidgetToValidate))
	{
		return;
	}

	// Let's loop through all widget properties containing the "Validate" or "ValidateIgnoreParent" metadata tag.
	for (TFieldIterator<FProperty> ClassPropertyIterator(InWidgetToValidate->GetClass()); ClassPropertyIterator; ++ClassPropertyIterator)
	{
		const FProperty* Property = *ClassPropertyIterator;

		if (Property->HasMetaData(TEXT("ValidateIgnoreParent")) && InWidgetToValidate->GetParent())
		{
			continue; // Skip nested widget validation
		}

		if (Property->HasMetaData(TEXT("Validate")) || Property->HasMetaData(TEXT("ValidateIgnoreParent")))
		{
			if (!IsPropertyValid(Property, InWidgetToValidate))
			{
				ReportValidationError(InCompileLog, InWidgetToValidate, Property);
			}
		}
	}
}

bool UEazyClassValidation::IsPropertyValid(const FProperty* Property, const UUserWidget* Widget)
{
	if (const FSoftObjectProperty* SoftObjectProp = CastField<FSoftObjectProperty>(Property))
	{
		return IsPropertyValid_SoftObject(SoftObjectProp, Widget);
	}
	
	if (const FSoftClassProperty* SoftClassProp = CastField<FSoftClassProperty>(Property))
	{
		return IsPropertyValid_SoftClass(SoftClassProp, Widget);
	}
	
	if (const FClassProperty* ClassProp = CastField<FClassProperty>(Property))
	{
		return IsPropertyValid_SubClass(ClassProp, Widget);
	}
	
	if (const FObjectPropertyBase* ObjectProp = CastField<FObjectPropertyBase>(Property))
	{
		return IsPropertyValid_Object(ObjectProp, Widget);
	}
	
	if (const FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		return IsPropertyValid_Struct(StructProp, Widget);
	}
	
	if (const FEnumProperty* EnumProp = CastField<FEnumProperty>(Property))
	{
		return IsPropertyValid_Enum(EnumProp, Widget);
	}

	ensureAlwaysMsgf(false, TEXT("Unsupported property type for validation on %s.%s"), *Widget->GetName(), *Property->GetName());
	return false;
}

bool UEazyClassValidation::IsPropertyValid_SoftObject(const FSoftObjectProperty* Property, const UUserWidget* Widget)
{
	const FSoftObjectPtr* SoftObject = Property->GetPropertyValuePtr_InContainer(Widget);
	return !SoftObject->IsNull();
}

bool UEazyClassValidation::IsPropertyValid_SoftClass(const FSoftClassProperty* Property, const UUserWidget* Widget)
{
	const FSoftObjectPtr* SoftClass = Property->GetPropertyValuePtr_InContainer(Widget);
	return !SoftClass->IsNull();
}

bool UEazyClassValidation::IsPropertyValid_SubClass(const FClassProperty* Property, const UUserWidget* Widget)
{
	const UClass* SubClass = Property->GetPropertyValue_InContainer(Widget).GetClass();
	return IsValid(SubClass);
}

bool UEazyClassValidation::IsPropertyValid_Object(const FObjectPropertyBase* Property, const UUserWidget* Widget)
{
	const UObject* Object = Property->GetObjectPropertyValue_InContainer(Widget);
	return IsValid(Object);
}

bool UEazyClassValidation::IsPropertyValid_Struct(const FStructProperty* Property, const UUserWidget* Widget)
{
	if (Property->Struct == TBaseStructure<FGameplayTag>::Get())
	{
		const FGameplayTag* Tag = Property->ContainerPtrToValuePtr<FGameplayTag>(Widget);
		return Tag->IsValid();
	}
	
	if (Property->Struct == TBaseStructure<FDataTableRowHandle>::Get())
	{
		const FDataTableRowHandle* DataTableRowHandle = Property->ContainerPtrToValuePtr<FDataTableRowHandle>(Widget);
		return !DataTableRowHandle->IsNull();
	}
	
	if (Property->Struct == TBaseStructure<FEazyUIActionData>::Get())
	{
		const FEazyUIActionData* UIActionData = Property->ContainerPtrToValuePtr<FEazyUIActionData>(Widget);
		return IsValid(UIActionData->InputAction);
	}

	ensureAlwaysMsgf(false, TEXT("Unsupported struct type for validation on %s.%s"), *Widget->GetName(), *Property->GetName());
	return false;
}

bool UEazyClassValidation::IsPropertyValid_Enum(const FEnumProperty* Property, const UUserWidget* Widget)
{
	const void* EnumValuePtr = Property->ContainerPtrToValuePtr<void>(Widget);
	const int32 EnumValue = Property->GetUnderlyingProperty()->GetSignedIntPropertyValue(EnumValuePtr);
	const UEnum* Enum = Property->GetEnum();
	
	FString EnumsToIgnore = Property->GetMetaData(TEXT("Validate"));
	if (EnumsToIgnore.IsEmpty())
	{
		EnumsToIgnore = Property->GetMetaData(TEXT("ValidateIgnoreParent"));
	}

	TArray<FString> InvalidNames;
	EnumsToIgnore.ParseIntoArray(InvalidNames, TEXT(","), true);

	for (const FString& Name : InvalidNames)
	{
		FString TrimmedName = Name;
		TrimmedName.TrimStartAndEndInline();
		FString FullName = TrimmedName.Contains(TEXT("::")) ? TrimmedName : Enum->GetName() + TEXT("::") + TrimmedName;

		const int32 InvalidValue = Enum->GetValueByNameString(FullName);
		if (InvalidValue == INDEX_NONE)
		{
			ensureAlwaysMsgf(false, TEXT("Unable to find enum value for validation on %s.%s. Check for typos: '%s'"), *Widget->GetName(), *Property->GetName(), *TrimmedName);
			return false;
		}

		if (EnumValue == InvalidValue)
		{
			return false;
		}
	}
	return true;
}

void UEazyClassValidation::ReportValidationError(IWidgetCompilerLog& CompileLog, const UUserWidget* Widget, const FProperty* Property)
{
	FString ClassName = Widget->GetClass()->GetName();
	ClassName.RemoveFromStart(TEXT("Default__"));
	ClassName.RemoveFromEnd(TEXT("_C"));

	CompileLog.Error(FText::Format(FText::FromString(TEXT("{0} has property {1} defined incorrectly.")), FText::FromString(ClassName), FText::FromString(Property->GetName())));
}
#endif // WITH_EDITOR
