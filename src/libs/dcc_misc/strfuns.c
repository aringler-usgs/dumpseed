#include <dcc_std.h>
#include <dcc_misc.h>

/* A few miscellaneous string functions */

/* Trim off all trailing white space (including \n) */

_SUB VOID TrimString(char *inst)
{

	int i;

	FOREVER {
		i = strlen(inst);
		if (i<=0) break;

		if (isspace(inst[i-1])) inst[i-1] = EOS;
		else break;
	}

}

/* Return pointer to first non-white character in string */

_SUB char *NonWhite(char *inst)
{

	FOREVER {
		if (*inst==EOS) return(inst);	/* Too far */

		if (!isspace(*inst)) return(inst);

		inst++;
	}
}

/* Make string upper case */

_SUB VOID Upcase(char *instr)
{

	while (*instr!='\0') {
		*instr=toupper(*instr);
		instr++;
	}
}

/* Make string lower case */

_SUB VOID Locase(char *instr)
{

	while (*instr!='\0') {
		*instr=tolower(*instr);
		instr++;
	}
}

/* Allocate string in memory, copy string there and return pointer to it */

_SUB char *alstr(char *instr)
{

	char *new;

	new = (char *) malloc(strlen(instr) + 1);

	if (new==NULL) bombout(EXIT_NOMEM,"alstr(%s) out mem",instr);

	strcpy(new,instr);

	return(new);
}

/* Return true if in1 can be found at beginning of in2 */

_SUB BOOL KeyFound(char *in1,char *in2)
{

	if (in1==NULL) return(FALSE);
	if (in2==NULL) return(FALSE);
	if (*in1=='\0') return(FALSE);
	if (*in2=='\0') return(FALSE);
	FOREVER {
		if (*in2=='\0') return(FALSE);
		if (*in1=='\0') return(TRUE);
		if (*in1++==*in2++) continue;
		return(FALSE);
	}
}
