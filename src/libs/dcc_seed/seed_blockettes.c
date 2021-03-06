#include <dcc_std.h>
#include <string.h>
#include <dcc_seed.h>
#include <dcc_misc.h>

int	DefBlkLen = 8192;		/* Default blockette buffer len */
int	DefBlkExt = 2048;		/* Default blockette extension */

/*********************Blockette Creation Support**********************/

/*
 *	
 *	Initialize a blockette structure for use (allocate the blockette
 *	buffer and set up the initial information) (Safe)
 *	
 */

_SUB BLOCKETTE *CreateBlockette()
{
	BLOCKETTE *InBlock;

	InBlock = (BLOCKETTE *) SafeAlloc(sizeof(BLOCKETTE));

	InBlock->BufLen = DefBlkLen;			/* Default */
	InBlock->BlkBuff = (char *) SafeAlloc(InBlock->BufLen);	
	                                           /* Allocate memory */

	InBlock->BlkType = 0;			/* Init remaining fields */
	InBlock->BlkVer = 0;
	InBlock->PackPos = 0;

	return(InBlock);		/* Success */
}

/*
 *	
 *	Set up a blockette structure to contain data
 *	
 */

_SUB VOID SetupBlockette(BLOCKETTE *InBlock,int BlockType,int BlockVer)
{

	InBlock->BlkType = BlockType;
	InBlock->BlkVer = BlockVer;
	InBlock->PackPos = 0;

}

/*
 *	
 *	Extend a blockette 
 *	
 */

_SUB VOID ExtendBlockette(BLOCKETTE *InBlock,ULONG Increments)
{

	int NewLen;
	char *NewBuff;

	if (Increments<=0) Increments = 1;	/* Default case */

	NewLen = InBlock->BufLen + (DefBlkExt * Increments);
	NewBuff = (char *) realloc(InBlock->BlkBuff, NewLen);

	if (NewBuff==NULL) 
	  bombout(EXIT_INSFMEM,"Could not extend blockette buffer to %d",
		  NewLen);

	InBlock->BlkBuff = NewBuff;
	InBlock->BufLen = NewLen;

}	

