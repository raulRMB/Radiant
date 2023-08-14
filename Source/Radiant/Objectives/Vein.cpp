#include "Vein.h"
#include "Net/UnrealNetwork.h"

bool AVein::OnExtracted(const float Amount)
{
	bool bSuccess = Amount <= RemainingRadianite;

	if(bSuccess)
	{
		RemainingRadianite -= Amount;
	}

	return bSuccess;
}

void AVein::SetEmpowered(const bool Empowered)
{
	bEmpowered = Empowered;
}

void AVein::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVein, RemainingRadianite);
	DOREPLIFETIME(AVein, bEmpowered);
}
