#include <dcc_std.h>

#if VAXC

#include iodef
#include ssdef
#include psldef
#include descrip
#include lnmdef
#include stdio

int   spawncommand(commstr)
char *commstr;
{

	ULONG	acflags = 0;
	ULONG	process_id,completion_status;
	ULONG	reterr;

	$DESCRIPTOR(spawnstring, "TEMP");

	spawnstring.dsc$w_length = strlen(commstr);
	spawnstring.dsc$a_pointer= commstr; 

	reterr = LIB$SPAWN(&spawnstring,	/* Command String */
			0,			/* Input file */
			0,			/* Output file */
			&acflags,		/* Flag bits */
			0,			/* Process name */
			&process_id,		/* Return Process id */
			&completion_status,	/* Return status */
			0,			/* Event flags */
			0,			/* AST address */
			0,			/* AST argument */
			0,			/* Prompt string */
			0);			/* Alternate CLI */
			
	if (reterr == EXIT_NORMAL) return(reterr);
	exit(reterr);

}

int	spawnjob(commstr,outputname,completion)
char *commstr,*outputname;
ULONG *completion;
{

	ULONG	acflags = 0;
	ULONG	process_id;
	ULONG	reterr;

	$DESCRIPTOR(spawnstring, "TEMP");
	$DESCRIPTOR(outdes, "TEMP");

	spawnstring.dsc$w_length = strlen(commstr);
	spawnstring.dsc$a_pointer= commstr; 

	outdes.dsc$w_length = strlen(outputname);
	outdes.dsc$a_pointer = outputname;

	reterr = LIB$SPAWN(&spawnstring,	/* Command String */
			0,			/* Input file */
			&outdes,		/* Output file */
			&acflags,		/* Flag bits */
			0,			/* Process name */
			&process_id,		/* Return Process id */
			completion,		/* Return status */
			0,			/* Event flags */
			0,			/* AST address */
			0,			/* AST argument */
			0,			/* Prompt string */
			0);			/* Alternate CLI */
			
	if (reterr == EXIT_NORMAL) return(reterr);
	exit(reterr);

}

#else

_SUB int	spawnjob(char *commstr,char *outputname,ULONG *completion)
{

	*completion = system(commstr);
	return(0);

}
#endif

#ifdef TESTMODE

main()
{

	spawncommand("DIR/DATE");
	printf("\nWe are still here\n");	

}
#endif
