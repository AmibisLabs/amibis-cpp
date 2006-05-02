#include <System/MultipleReferencedData.h>

MultipleReferencedData::MultipleReferencedData(MethodForRelease method_for_release)
{
  methodForRelease = method_for_release;
  Init();
}

MultipleReferencedData::~MultipleReferencedData()
{}
 
void MultipleReferencedData::Init()
{
  NbCurrentRef = 0;
}

void MultipleReferencedData::ReleaseData(MultipleReferencedData* multiple_referenced_data)
{ 
  multiple_referenced_data->RemoveRef(); 
  if(!( multiple_referenced_data->IsStillUseful()))
    {
      MethodForRelease method_for_release = multiple_referenced_data->methodForRelease;
      if(method_for_release == NULL)
	{
	  delete multiple_referenced_data;
	}
      else
	{
	  method_for_release(multiple_referenced_data);
	}
    }
}
