/*
 *				a t o f . c
 */

/*)LIBRARY
*/

#ifdef	DOCUMENTATION
title	atof	ascii to double conversion
index	ascii to double conversion
usage
	.s
	double d, atof();
	.br
	char *buffer;
	.br
	d = atof(buffer);
	.s
description
	.s
	This program is a (temporary?) replacement for the one of the
	same name described on page 5-52 of the Wizard document on the
	11-SP-18 tape. The type of conversion from ascii string to
	double floating point number given there is reproduced here.
	The BNF for numbers which can be decoded by that routine is :-
	.br
	<number> := <sign> <real number>
	.br
	<sign> := + | - | <empty>
	.br
	<real number> := <decimal number> | <decimal number> <exponent> |
	<exponent>
	.br
	<decimal number> := <integer> | <integer> . | . <integer> |
	<integer> . <integer>
	.br
	<integer> := <digit> | <integer> <digit>
	.br
	<exponent> := <explet> <sign> <integer>
	.br
	<explet> := E | e | D | d
	.br
	<digit> := 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
	.br
	This routine differs slightly from the above, in that a number may not
	start at the exponent part without a leading digit.
	That means that the description of a real nuber is now
	<real number> := <decimal number> | <decimal number> <exponent>
	The reason for doing it this way is that it stops scanf("%f", &x)
	eating its way into a following string if that string starts with
	a 'd' or an 'e'. It will still do that if the following string
	begins with a '.'. K & R does not define what atof's idea
	of a floating number is, but the above definition is closer
	than the DECUS C version to their definition of a floating constant.
	The conversion stops on the first non-legal character, but
	unlike the original version this one prints an error message
	and the offending string through the cmathe error reporting system.
	.s
diagnostics
	.s
	If a non-legal character is encountered the message 'illegal character
	in floating input string', followed by the entire string in the
	buffer argument, is written to stderr.  The value of the number
	built up to that point is returned.
	.s
	If the input produces a number which would result in floating
	overflow the message 'atof input too large', followed by the entire
	string in the buffer argument, is written to stderr. The value 
	HUGE is returned.
	.s
internal
	.s
	Algorithm is straightforward, see comments in source code for details.
	.s
author
	.s
	Hamish Ross.
	.s
date
	.s
	22-Aug-84 original version
	.b
	25-Mar-85 insist numbers start with a digit not 'e' or 'd'.
#endif

#include <math.h>
#define PZERO 38			/* index of 1e0 in powten[]	*/
#define PMAX 76				/* highest index of powten[]	*/
#define HUGE_NOR 1.70141183460469228		/* normalised HUGE	*/
double atof(s) /* convert string s to double */
char s[];
{
    extern double powten[];
    double val;
    int i, j, sign, esign, ex, ex1;

/* skip white space */
    for (i=0; s[i]==' ' || s[i]=='\n' || s[i]=='\t'; i++);
/* set sign to be +1 or -1 */
    sign = 1;
    if (s[i] == '+' || s[i] == '-')    /* sign */
	sign = (s[i++]=='+') ? 1 : -1;
/* read digits and build value for as long as there are digits */
    ex = PZERO - 1;
    for (val = 0.0, j = PZERO; s[i] >= '0' && s[i] <= '9'; i++) {
	if (j >= 0)
	    val += powten[j] * (s[i] - '0');
	j--;
	ex++;
    }
/* if first non-digit is decimal point skip it and keep going */
/* if it is not a decimal point we fall through to test for exponent */
    if (s[i] == '.')
	i++;
/* continue to build value while the digits keep coming */
    for (; s[i] >= '0' && s[i] <= '9'; i++){
	if (j >= 0)
	    val += powten[j] * (s[i] - '0');
	j--;
    }
/* if non-digit was an exponent flag deal with exponent */
    if (s[i]=='e' || s[i]=='E' || s[i]=='d' || s[i]=='D') {
	i++;
	esign = 1;
/* check for explicit + or - sign in exponent and deal with it */
	if (s[i] == '+' || s[i] == '-')
	    esign = (s[i++]=='+') ? 1 : -1;
	for (ex1 = 0; s[i] >= '0' && s[i] <= '9'; i++)
	    ex1 = 10 * ex1 + s[i] - '0';
	ex += ex1 * esign;
    }
/* incorporate exponent into val */
    if (ex < 0)
	val = 0.0;
    else if (ex < PMAX || (ex == PMAX && val < HUGE_NOR))
	val *= powten[ex];
    else {
	cmemsg(FP_BIGI, s);
	val = HUGE;
    }
/* check that we have an acceptable termination; if not call error system
   before returning with what we have */
    if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != '\0')
	cmemsg(FP_BADC, s);
    return(sign * val);
}
