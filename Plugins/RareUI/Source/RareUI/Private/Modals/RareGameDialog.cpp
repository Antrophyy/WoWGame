#include "Modals/RareGameDialog.h"

#include "Modals/RareGameDialogDescriptor.h"

void URareGameDialog::SetupDialog(URareGameDialogDescriptor* Descriptor)
{
	if (Descriptor->ErrorPriority > ERareModalsPriority::None)
	{
		DialogDescriptor = Descriptor;
	}
}