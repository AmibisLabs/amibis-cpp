#include <System/MultipleReferencedData.h>

using namespace Omiscid;

MultipleReferencedData::MultipleReferencedData(MethodForRelease method_for_release /* = NULL */ )
{
	methodForRelease = method_for_release;
	Init();
}

MultipleReferencedData::~MultipleReferencedData()
{}

void MultipleReferencedData::Init()
{
	SmartLocker SL_Myself(*this);

	NbCurrentRef = 0;
}

void MultipleReferencedData::ReleaseData(MultipleReferencedData* multiple_referenced_data)
{
	SmartLocker SL_Myself(*multiple_referenced_data);

	multiple_referenced_data->InternalRemoveRef();
	if ( multiple_referenced_data->InternalIsStillUseful() == false )
	{
		// Check if a specific Method had been set to destroy the object
		MethodForRelease method_for_release = multiple_referenced_data->methodForRelease;
		if ( method_for_release == (MethodForRelease)NULL )
		{
			SL_Myself.Unlock();
			delete multiple_referenced_data;
		}
		else
		{
			SL_Myself.Unlock();
			method_for_release( multiple_referenced_data );
		}
	}
}

void MultipleReferencedData::AddRef()
{
	SmartLocker SL_Myself(*this);
	InternalAddRef();
//	fprintf( stderr, "AddRef %p : %u\n", (void*)this, this->NbCurrentRef );
}

void MultipleReferencedData::RemoveRef()
{
	SmartLocker SL_Myself(*this);
	InternalRemoveRef();
//	fprintf( stderr, "RemoveRef %p : %u\n", (void*)this, this->NbCurrentRef );
}

bool MultipleReferencedData::IsStillUseful() const
{
	SmartLocker SL_Myself(*this);
	return InternalIsStillUseful();
}

