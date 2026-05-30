#pragma once

#include "EazyViewModelInstanceHandle.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Templates/SubclassOf.h"
#include "EazyViewModelSubsystem.generated.h"

class UEazyViewModelBase;

struct FEazyMultipleInstanceViewModelEntry
{
	TWeakObjectPtr<UEazyViewModelBase> ViewModelInstance;
	TWeakObjectPtr<UObject> ContextObject;
	TSubclassOf<UEazyViewModelBase> ViewModelClass;
};

UCLASS(ClassGroup=MVVM, Category=MVVM)
class EAZYUIMVVM_API UEazyViewModelSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;
	
	void DumpViewModels() const;

private:
	UEazyViewModelBase* GetOrCreateViewModel(UObject* ContextObject, TSubclassOf<UEazyViewModelBase> ViewModelClass);
	void ReleaseViewModel(UObject* ContextObject, TSubclassOf<UEazyViewModelBase> ViewModelClass);
	
private:
	UEazyViewModelBase* CreateViewModelInstance(const TSubclassOf<UEazyViewModelBase> ViewModelClass);
	UEazyViewModelBase* GetExistingViewModel(UObject* ContextObject, TSubclassOf<UEazyViewModelBase> ViewModelClass);
	void DestroyViewModel(UEazyViewModelBase* ViewModel) const;

	// Dump helper methods
	void DumpMultipleInstanceViewModels() const;
	void DumpMultipleInstanceViewModelEntry(const TSubclassOf<UEazyViewModelBase>& ViewModelClass, const TArray<const FEazyMultipleInstanceViewModelEntry*>& Entries) const;
	void DumpSharedViewModels() const;
	void DumpSharedViewModelEntry(const TSubclassOf<UEazyViewModelBase>& ViewModelClass, const FEazyViewModelInstanceHandle& InstanceHandle) const;
	void DumpPersistentViewModels() const;
	
	// Map of currently active single-instance, non-persistent ViewModels. The subsystem will try to reuse these when possible.
	TMap<TSubclassOf<UEazyViewModelBase>, FEazyViewModelInstanceHandle> ViewModelInstanceHandles;
	
	// Map of currently active persistent ViewModels. Once created, these will never be destroyed and will be reused whenever requested.
	UPROPERTY(Transient)
	TMap<TSubclassOf<UEazyViewModelBase>, TObjectPtr<UEazyViewModelBase>> PersistentViewModelInstances;

	// List of currently active unique (multiple-instance) ViewModels and their owner widget/context object.
	TArray<FEazyMultipleInstanceViewModelEntry> MultipleInstanceViewModelEntries;
	
	friend class IEazyViewModelWidgetInterface;
};

