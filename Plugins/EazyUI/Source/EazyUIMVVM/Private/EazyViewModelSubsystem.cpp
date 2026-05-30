#include "EazyViewModelSubsystem.h"

#include "EazyViewModelBase.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "LogEazyUIMVVM.h"
#include "Engine/World.h"

static FAutoConsoleCommandWithWorld DumpViewModelsCommand(
	TEXT("EazyUI.DumpViewModels"),
	TEXT("Dumps all alive ViewModels and their referencing objects to the log."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](const UWorld* World)
	{
		if (!IsValid(World))
		{
			return;
		}

		const UGameInstance* GameInstance = World->GetGameInstance();
		if (!IsValid(GameInstance))
		{
			return;
		}

		const UEazyViewModelSubsystem* ViewModelSubsystem = ULocalPlayer::GetSubsystem<UEazyViewModelSubsystem>(GameInstance->GetFirstLocalPlayerController()->GetLocalPlayer());
		if (IsValid(ViewModelSubsystem))
		{
			ViewModelSubsystem->DumpViewModels();
		}
	})
);

void UEazyViewModelSubsystem::DestroyViewModel(UEazyViewModelBase* ViewModel) const
{
	if (IsValid(ViewModel))
	{
		UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Destroying ViewModel: %s"), *ViewModel->GetClass()->GetName());
		ViewModel->DeinitializeViewModel();
	}
}

void UEazyViewModelSubsystem::Deinitialize()
{
	UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Deinitializing ViewModel Subsystem - cleaning up all ViewModels"));

	// Deinitialize all persistent ViewModels
	for (TPair<TSubclassOf<UEazyViewModelBase>, TObjectPtr<UEazyViewModelBase>>& Pair : PersistentViewModelInstances)
	{
		DestroyViewModel(Pair.Value.Get());
	}

	PersistentViewModelInstances.Empty();

	// Deinitialize all shared ViewModels
	for (TPair<TSubclassOf<UEazyViewModelBase>, FEazyViewModelInstanceHandle>& Pair : ViewModelInstanceHandles)
	{
		DestroyViewModel(Pair.Value.ViewModelInstance.Get());
	}

	ViewModelInstanceHandles.Empty();
	MultipleInstanceViewModelEntries.Empty();

	Super::Deinitialize();
}

UEazyViewModelBase* UEazyViewModelSubsystem::GetOrCreateViewModel(UObject* ContextObject, const TSubclassOf<UEazyViewModelBase> ViewModelClass)
{
	if (!IsValid(ViewModelClass))
	{
		return nullptr;
	}

	const UEazyViewModelBase* CDO = ViewModelClass->GetDefaultObject<UEazyViewModelBase>();

	if (CDO->IsPersistent())
	{
		if (PersistentViewModelInstances.Contains(ViewModelClass))
		{
			UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Retrieving existing persistent ViewModel: %s"), *ViewModelClass->GetName());
			return PersistentViewModelInstances[ViewModelClass].Get();
		}

		UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Creating new persistent ViewModel: %s"), *ViewModelClass->GetName());
		UEazyViewModelBase* ViewModel = CreateViewModelInstance(ViewModelClass);
		PersistentViewModelInstances.Add(ViewModelClass, ViewModel);

		return ViewModel;
	}

	if (CDO->AlwaysCreateNewInstance())
	{
		UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Creating unique instance of ViewModel: %s for context: %s"), *ViewModelClass->GetName(), *ContextObject->GetName());
		UEazyViewModelBase* ViewModel = CreateViewModelInstance(ViewModelClass);

		ViewModel->OnDestroyedEvent.AddUObject(this, &UEazyViewModelSubsystem::ReleaseViewModel, ContextObject, ViewModelClass);
		
		FEazyMultipleInstanceViewModelEntry NewMultipleInstanceEntry;
		NewMultipleInstanceEntry.ViewModelInstance = ViewModel;
		NewMultipleInstanceEntry.ContextObject = ContextObject;
		NewMultipleInstanceEntry.ViewModelClass = ViewModelClass;
		MultipleInstanceViewModelEntries.Emplace(MoveTemp(NewMultipleInstanceEntry));

		return ViewModel;
	}

	if (ViewModelInstanceHandles.Contains(ViewModelClass))
	{
		UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Retrieving existing ViewModel: %s for context: %s"), *ViewModelClass->GetName(), *ContextObject->GetName());
		return GetExistingViewModel(ContextObject, ViewModelClass);
	}
	else
	{
		UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Creating new ViewModel: %s for context: %s"), *ViewModelClass->GetName(), *ContextObject->GetName());
		UEazyViewModelBase* ViewModel = CreateViewModelInstance(ViewModelClass);

		FEazyViewModelInstanceHandle NewInstanceHandle;
		NewInstanceHandle.ViewModelInstance = ViewModel;
		NewInstanceHandle.ReferencingObjects.Emplace(ContextObject);

		ViewModelInstanceHandles.Add(ViewModelClass, NewInstanceHandle);

		return ViewModel;
	}
}

void UEazyViewModelSubsystem::ReleaseViewModel(UObject* ContextObject, const TSubclassOf<UEazyViewModelBase> ViewModelClass)
{
	if (PersistentViewModelInstances.Contains(ViewModelClass))
	{
		// Persistent ViewModels are never destroyed until the subsystem itself is deinitialized, so we don't need to do anything here.
		UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Ignoring release for persistent ViewModel: %s"), *ViewModelClass->GetName());
		return;
	}

	if (ViewModelInstanceHandles.Contains(ViewModelClass))
	{
		FEazyViewModelInstanceHandle& InstanceHandle = ViewModelInstanceHandles[ViewModelClass];
		InstanceHandle.ReferencingObjects.RemoveAll([ContextObject](const TWeakObjectPtr<UObject>& ReferencingObject)
		{
			return !ReferencingObject.IsValid() || ReferencingObject.Get() == ContextObject;
		});

		if (InstanceHandle.ReferencingObjects.Num() == 0)
		{
			UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Releasing ViewModel with no remaining references: %s"), *ViewModelClass->GetName());
			DestroyViewModel(InstanceHandle.ViewModelInstance.Get());
			ViewModelInstanceHandles.Remove(ViewModelClass);
		}
		else
		{
			UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Releasing reference to ViewModel: %s (remaining references: %d)"), *ViewModelClass->GetName(), InstanceHandle.ReferencingObjects.Num());
		}
	}

	// Remove matching or stale multiple-instance ViewModel entries and deinitialize them
	for (int32 Index = MultipleInstanceViewModelEntries.Num() - 1; Index >= 0; --Index)
	{
		const FEazyMultipleInstanceViewModelEntry& Entry = MultipleInstanceViewModelEntries[Index];

		const bool bHasMatchingContextObject = Entry.ContextObject.Get() == ContextObject;
		const bool bHasMatchingClass = Entry.ViewModelClass == ViewModelClass;
		const bool bIsStaleEntry = !Entry.ViewModelInstance.IsValid() || !Entry.ContextObject.IsValid();

		if (bIsStaleEntry || (bHasMatchingContextObject && bHasMatchingClass))
		{
			if (bIsStaleEntry)
			{
				UE_LOG(LogEazyUIMVVM, Verbose, TEXT("Cleaning up stale unique ViewModel entry: %s"), *ViewModelClass->GetName());
			}
			
			Entry.ViewModelInstance->OnDestroyedEvent.RemoveAll(this);
			MultipleInstanceViewModelEntries.RemoveAt(Index);
		}
	}
}

UEazyViewModelBase* UEazyViewModelSubsystem::CreateViewModelInstance(const TSubclassOf<UEazyViewModelBase> ViewModelClass)
{
	UEazyViewModelBase* CreatedViewModel = NewObject<UEazyViewModelBase>(this, ViewModelClass);
	CreatedViewModel->InitializeViewModel(GetLocalPlayer());

	return CreatedViewModel;
}

UEazyViewModelBase* UEazyViewModelSubsystem::GetExistingViewModel(UObject* ContextObject, const TSubclassOf<UEazyViewModelBase> ViewModelClass)
{
	FEazyViewModelInstanceHandle& InstanceHandle = ViewModelInstanceHandles[ViewModelClass];
	if (InstanceHandle.ViewModelInstance.IsValid())
	{
		InstanceHandle.ReferencingObjects.Emplace(ContextObject);
		return InstanceHandle.ViewModelInstance.Get();
	}

	return nullptr;
}

void UEazyViewModelSubsystem::DumpViewModels() const
{
	DumpMultipleInstanceViewModels();
	DumpSharedViewModels();
	DumpPersistentViewModels();
}

void UEazyViewModelSubsystem::DumpMultipleInstanceViewModels() const
{
	TMap<TSubclassOf<UEazyViewModelBase>, TArray<const FEazyMultipleInstanceViewModelEntry*>> MultipleInstanceEntriesByClass;

	for (const FEazyMultipleInstanceViewModelEntry& Entry : MultipleInstanceViewModelEntries)
	{
		if (!Entry.ViewModelInstance.IsValid())
		{
			continue;
		}

		MultipleInstanceEntriesByClass.FindOrAdd(Entry.ViewModelClass).Add(&Entry);
	}

	UE_LOG(LogEazyUIMVVM, Log, TEXT("--- Unique Instance ViewModels (%d) ---"), MultipleInstanceViewModelEntries.Num());
	for (const TPair<TSubclassOf<UEazyViewModelBase>, TArray<const FEazyMultipleInstanceViewModelEntry*>>& Pair : MultipleInstanceEntriesByClass)
	{
		DumpMultipleInstanceViewModelEntry(Pair.Key, Pair.Value);
	}
}

void UEazyViewModelSubsystem::DumpMultipleInstanceViewModelEntry(const TSubclassOf<UEazyViewModelBase>& ViewModelClass, const TArray<const FEazyMultipleInstanceViewModelEntry*>& Entries) const
{
	const FString ViewModelClassName = IsValid(ViewModelClass) ? ViewModelClass->GetName() : TEXT("Invalid Class");
	UE_LOG(LogEazyUIMVVM, Log, TEXT("    [%s] Unique instances: %d"), *ViewModelClassName, Entries.Num());

	UE_LOG(LogEazyUIMVVM, Log, TEXT("        WBPs with unique VM:"));
	for (const FEazyMultipleInstanceViewModelEntry* Entry : Entries)
	{
		if (Entry == nullptr)
		{
			continue;
		}

		const UObject* ContextObject = Entry->ContextObject.Get();
		const FString WidgetObjectName = ContextObject->GetName();
		UE_LOG(LogEazyUIMVVM, Log, TEXT("            - %s"), *WidgetObjectName);
	}
}

void UEazyViewModelSubsystem::DumpSharedViewModels() const
{
	UE_LOG(LogEazyUIMVVM, Log, TEXT("--- Active ViewModels (%d) ---"), ViewModelInstanceHandles.Num());
	for (const TPair<TSubclassOf<UEazyViewModelBase>, FEazyViewModelInstanceHandle>& Pair : ViewModelInstanceHandles)
	{
		const TSubclassOf<UEazyViewModelBase>& ViewModelClass = Pair.Key;
		const FEazyViewModelInstanceHandle& InstanceHandle = Pair.Value;

		if (!InstanceHandle.ViewModelInstance.IsValid())
		{
			continue;
		}

		DumpSharedViewModelEntry(ViewModelClass, InstanceHandle);
	}
}

void UEazyViewModelSubsystem::DumpSharedViewModelEntry(const TSubclassOf<UEazyViewModelBase>& ViewModelClass, const FEazyViewModelInstanceHandle& InstanceHandle) const
{
	const FString ClassName = IsValid(ViewModelClass) ? ViewModelClass->GetName() : TEXT("Invalid Class");
	UE_LOG(LogEazyUIMVVM, Log, TEXT("    [%s]"), *ClassName);

	UE_LOG(LogEazyUIMVVM, Log, TEXT("        Used by (%d):"), InstanceHandle.ReferencingObjects.Num());
	for (const TWeakObjectPtr<UObject>& ReferencingObject : InstanceHandle.ReferencingObjects)
	{
		UE_LOG(LogEazyUIMVVM, Log, TEXT("            - %s"), *ReferencingObject->GetName());
	}
}

void UEazyViewModelSubsystem::DumpPersistentViewModels() const
{
	UE_LOG(LogEazyUIMVVM, Log, TEXT("--- Persistent ViewModels (%d) ---"), PersistentViewModelInstances.Num());
	for (const TPair<TSubclassOf<UEazyViewModelBase>, TObjectPtr<UEazyViewModelBase>>& Pair : PersistentViewModelInstances)
	{
		const TSubclassOf<UEazyViewModelBase>& ViewModelClass = Pair.Key;
		const UEazyViewModelBase* ViewModelInstance = Pair.Value.Get();

		if (!IsValid(ViewModelInstance))
		{
			continue;
		}

		const FString ClassName = IsValid(ViewModelClass) ? ViewModelClass->GetName() : TEXT("Invalid Class");
		UE_LOG(LogEazyUIMVVM, Log, TEXT("    [%s] (Persistent - owned by subsystem)"), *ClassName);
	}
}
