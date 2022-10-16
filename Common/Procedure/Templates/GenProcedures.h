#pragma once
#include "Types.h"
#include <windows.h>
#include "DBBind.h"

{%- macro gen_procedures() -%} {% include 'Procedure.h' %} {% endmacro %}

/*---------------------------------------------------------------------------------------------
								PROCEUDRE AUTO CREATEA
----------------------------------------------------------------------------------------------*/

namespace ProcedureManager
{
	{{gen_procedures() | indent}}
};