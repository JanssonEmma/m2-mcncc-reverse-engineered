// Complete ORIGINAL owning lifecycle closure only. Other full APIs declared, no fake Analyze/Eval.
#include "Poly.h"
#include "Constants.h"
#include <cmath>
#include <cctype>
#include <cstdlib>
using namespace std;
namespace poly_detail {
static int signed32(unsigned int n) { return n<=0x7fffffffU?(int)n:-1-(int)(0xffffffffU-n); }
static int add(int a,int b) { return signed32((unsigned int)a+(unsigned int)b); }
static int decimal(int a,int digit) { return signed32((unsigned int)a*10U+(unsigned int)digit); }
}
CPoly::CPoly()
{
    uiLookPos=0;
    ErrorOccur=true;
    lSymbol.clear();
    STSize=0;
    lSymbol.reserve(50);

    init();
}

CPoly::~CPoly()
{
    Clear();
}

void CPoly::Clear()
{
    int i;

    tokenBase.clear();
    numBase.clear();

    for (i = 0;i < STSize; ++i)
    {
	if (lSymbol[i]) delete lSymbol[i];
	lSymbol[i]=NULL;
    }
    
    lSymbol.clear();
    SymbolIndex.clear();
    STSize=0;
}

int CPoly::find(const string & s)
{
	int l, m, r;

	l = 0;
	r = poly_detail::add(STSize,-1);

	while (l <= r)
	{
		m = poly_detail::add(l,r) >> 1;

		if (lSymbol[SymbolIndex[m]]->strlex == s)
			return SymbolIndex[m];
		else if (lSymbol[SymbolIndex[m]]->strlex < s)
			l = poly_detail::add(m,1);
		else
			r = poly_detail::add(m,-1);
	}
	return -1;
}

int CPoly::insert(const string & s, int tok)
{
	int i;
	bool bAdded=false;

	lSymbol.push_back(new CSymTable(tok,s));
	for (i=0;i<STSize;i++)
	{
		if (s<lSymbol[SymbolIndex[i]]->strlex)
		{
			SymbolIndex.insert(SymbolIndex.begin()+i,STSize);
			bAdded=true;
			break;
		}
	}
	if (!bAdded)
	{
		
		SymbolIndex.push_back(STSize);
	}
	const int index=STSize;
	STSize=poly_detail::add(STSize,1);
	return index;
}

int CPoly::SetVar(const string & strName, double dVar)
{
    
    if (ErrorOccur) return false;
    int index = find(strName);
    if (index == -1) return false;
    CSymTable * stVar = lSymbol[((index))];
    stVar->dVal = dVar;
    return true;
}

void CPoly::init()
{
    insert("min",MINF);
    insert("max",MAXF);
    insert("number", IRAND);
    insert("irandom", IRAND);
    insert("irand", IRAND);
    insert("frandom",FRAND);
    insert("frand",FRAND);
    insert("rt",ROOT);
    insert("sqrt",ROOT);
    insert("cos",COS);
    insert("sin",SIN);
    insert("tan",TAN);
    insert("cot",COT);
    insert("csc",CSC);
    insert("cosec",COSEC);
    insert("sec",SEC);
    insert("pi",PI);
    SetVar("pi",3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068);
    insert("e",EXP);
    SetVar("e",2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427);
    insert("log",LOG);
    insert("ln",LN);
    insert("log10",LOG10);
    insert("abs",ABS);
    insert("mod",MOD);
    insert("floor", FLOOR);
    insert("sign",SIGN);
}

// Complete remaining original CPoly bodies, appended to actual owning closure.
extern "C" long random(void);

double _random()
{
    return random() / 2147483648.0;
}

static int TruncateRandomResult(long double value)
{
    // FISTP yields the signed integer-indefinite value outside the 32-bit range.
    if (!(value > -2147483649.0L && value < 2147483648.0L))
        return -2147483647 - 1;
    return static_cast<int>(value);
}

int CPoly::my_irandom(double start, double end)
{
    const long double first = start;
    const long double span = static_cast<long double>(end) - first;
    const int origin = TruncateRandomResult(first + 0.5L);
    const int count = poly_detail::add(TruncateRandomResult(span + 0.5L), 1);
    return TruncateRandomResult(static_cast<long double>(_random()) * count + origin);
}

double CPoly::my_frandom(double start, double end)
{
    const long double factor = _random();
    return static_cast<double>(factor * (static_cast<long double>(end) - start) + start);
}

void CPoly::SetStr(const string & str)
{
    strData = str;
}

double CPoly::Eval()
{
	int stNow;
	double save[MAXSTACK],t;
	int iSp=0;
	if (ErrorOccur) 
	{
		 
		return 0;
	}

	vector<int>::iterator pos = tokenBase.begin();
	vector<double>::iterator posn = numBase.begin();
	while (pos != tokenBase.end())
	{

		stNow=*pos;
		++pos;
		switch (stNow)
		{
			case NUM:
				save[iSp++]=*posn++; break;
			case ID:
				save[iSp++]=
					lSymbol[ *pos ]->dVal; 
				pos++;
				break;
				
			case PLU:
				iSp--;
				save[iSp-1] = static_cast<double>(static_cast<long double>(save[iSp-1]) + save[iSp]); break;
				
			case MIN:
				iSp--;
				save[iSp-1] = static_cast<double>(static_cast<long double>(save[iSp-1]) - save[iSp]); break;
				
			case MUL:
				iSp--;
				save[iSp-1] = static_cast<double>(static_cast<long double>(save[iSp-1]) * save[iSp]); break;
				
			case MOD:
				iSp--;
				if (save[iSp]==0) 
				{
					
					return 0;
				}
				save[iSp-1]=fmod(save[iSp-1],save[iSp]); break;
				
			case DIV:
				iSp--;
				if (save[iSp]==0) 
				{
					
					return 0;
				}
				save[iSp-1] = static_cast<double>(static_cast<long double>(save[iSp-1]) / save[iSp]); break;
				
			case POW:
				iSp--;
				save[iSp-1]=pow(save[iSp-1],save[iSp]); break;
			case ROOT:
				if (save[iSp-1]<0) 
				{
					
					return 0;
				}
				save[iSp-1]=sqrt(save[iSp-1]); break;
			case COS:
				save[iSp-1]=cos(save[iSp-1]); break;
			case SIN:
				save[iSp-1]=sin(save[iSp-1]); break;
			case SIGN:
				if (save[iSp-1] == 0.0) save[iSp-1] = 0.0;
				else if (save[iSp-1] < 0.0) save[iSp-1] = -1.0;
				else save[iSp-1] = 1.0f;
				break;
			case TAN:
				if (!(t=cos(save[iSp-1]))) 
				{
					
					return 0;
				}
				save[iSp-1]=tan(save[iSp-1]); break;
			case CSC:
				if (!(t=sin(save[iSp-1]))) 
				{
					
					return 0;
				}
				save[iSp-1] = static_cast<double>(1.0L / t); break;
			case SEC:
				if (!(t=cos(save[iSp-1]))) 
				{
					
					return 0;
				}
				save[iSp-1] = static_cast<double>(1.0L / t); break;
			case COT:
				if (!(t=sin(save[iSp-1]))) 
				{
					
					return 0;
				}
				save[iSp-1] = static_cast<double>(static_cast<long double>(cos(save[iSp-1])) / t); break;
			case LN:
				if (save[iSp-1]<=0) 
				{
					
					return 0;
				}
				save[iSp-1]=log(save[iSp-1]); break;
			case LOG10:
				if (save[iSp-1]<=0) 
				{
					
					return 0;
				}
				save[iSp-1]=log10(save[iSp-1]); break;
			case LOG:
				if (save[iSp-1]<=0) 
				{
					
					return 0;
				}
				if (save[iSp-2]<=0 || save[iSp-2]==1) 
				{
					
					return 0;
				}

				{
					const double numerator = log(save[iSp-1]);
					const long double denominator = log(save[iSp-2]);
					save[iSp-2] = static_cast<double>(numerator / denominator);
				}
				iSp--;
				break;
			case ABS:	save[iSp-1]=fabs(save[iSp-1]); break;
			case FLOOR:	save[iSp-1]=floor(save[iSp-1]); break;
			case IRAND:
						save[iSp-2]=my_irandom(save[iSp-2],save[iSp-1]);
						iSp--;
						break;
			case FRAND:
						save[iSp-2]=my_frandom(save[iSp-2],save[iSp-1]);
						iSp--;
						break;
			case MINF:
						save[iSp-2]=(save[iSp-2]<save[iSp-1])?save[iSp-2]:save[iSp-1];
						iSp--;
						break;
			case MAXF:
						save[iSp-2]=(save[iSp-2]>save[iSp-1])?save[iSp-2]:save[iSp-1];
						iSp--;
						break;
						
			default:
						return 0;
						
		}
	}
	return save[iSp-1];
}

int CPoly::Analyze(const char * pszStr)
{
    if (pszStr)
	SetStr(pszStr);

    if (0 == strData.length())
	return true;

    ErrorOccur = false;
    uiLookPos = 0;
    iLookAhead = lexan();

    expr();

    if (tokenBase.empty()) 
    {
	
	return false;
    }

    return !ErrorOccur;
}

void CPoly::expr() 
{
    int t;

    switch (iLookAhead)
    {
	case '+':
	case '-':
	    uiLookPos--;
	    iLookAhead = NUM;
	    iNumToken = iToken = 0;

    }

    term();

    while (!ErrorOccur)
    {
	switch (iLookAhead)
	{
	    case '+':
	    case '-':
		t=iLookAhead;
		match(t);
		term();
		emit(t,NONE);
		continue;
	    case EOS: case ')': case ',': return;
	    default:
					  error();
					  
					  return;
	}
    }
}

void CPoly::error()
{
    iErrorPos=uiLookPos;
    ErrorOccur=true;
}

int CPoly::lexan()
{
    int t;
    double tt;

    while (uiLookPos < strData.size())
    {
	if (strData[uiLookPos] == ' ' || strData[uiLookPos] == '\t')
	    ;
	else if (isdigit(strData[uiLookPos]))
	{
	    t = 0;
	    for (;uiLookPos<strData.size();uiLookPos++)
	    {
		if (isdigit(strData[uiLookPos]))
		    t = poly_detail::decimal(t,strData[uiLookPos]-'0');
		else
		    break;
	    }
	    iToken=t;
	    tt=0.1;
	    iNumToken=0;
	    if (uiLookPos<strData.size() && strData[uiLookPos]=='.')
	    {
		uiLookPos++;
		for (;uiLookPos<strData.size();uiLookPos++,tt*=0.1)
		{
		    if (isdigit(strData[uiLookPos]))
			iNumToken+=tt*(strData[uiLookPos]-'0');
		    else
			break;
		}
	    }
	    iNumToken+=iToken;
	    return NUM;
	}
	else if (isalpha(strData[uiLookPos]))
	{
	    string localSymbol("");
	    while (uiLookPos<strData.size() && isalpha(strData[uiLookPos]))
	    {
		localSymbol+=strData[uiLookPos];
		uiLookPos++;
	    }
	    iToken= find(localSymbol);
	    if (iToken==-1)
	    {
		iToken=insert(localSymbol,ID);
	    }
	    return lSymbol[((iToken))]->token;
	}
	else
	{
	    iToken=0;
	    return strData[uiLookPos++];
	}
	uiLookPos++;
    }
    return EOS;
}

void CPoly::term()
{
    int t;
    factor();
    while (!ErrorOccur)
    {
	switch (iLookAhead)
	{
	    case '*':
	    case '/':
	    case '%':
		t=iLookAhead;
		match(t);
		factor();
		emit(t,NONE);
		continue;
	    default:
		return;
	}
    }
}

void CPoly::factor()
{
    int t;
    expo();
    while (!ErrorOccur)
    {
	switch (iLookAhead)
	{
	    case '^':
		t=iLookAhead;
		match(t);
		expo();
		emit(t,NONE);
		continue;
	    default:
		return;
	}
    }
}

void CPoly::expo()
{
    int t;
    switch (iLookAhead)
    {
	case '(':
	    match('('); expr(); match(')'); break;
	case NUM:
	    emit(NUM, iToken); match(NUM); break;
	case ID:
	    emit(ID,(int)(iToken)); match(ID); break;
	case ROOT:
	case SIN:
	case COT:
	case TAN:
	case CSC:
	case SEC:
	case LN:
	case LOG10:
	case COS:
	case ABS:
	case FLOOR:
	case SIGN:
	    t=iLookAhead;
	    match(iLookAhead); match('('); expr(); match(')'); emit(t,iToken);
	    break;

	case LOG:
	case MINF:
	case MAXF:
	case IRAND:
	case FRAND:
	case MOD:
	    t=iLookAhead;
	    match(iLookAhead); match('('); expr(); match(','); expr(); match(')'); emit(t,iToken);
	    break;
	case EOS:
	    break;
	default:
	    error();
	    
    }
}

void CPoly::match(int t)
{
    if (iLookAhead==t) iLookAhead=lexan(); else error();
}

void CPoly::emit(int t, int tval)
{
    switch (t)
    {
	case '+': 
	    tokenBase.push_back(PLU);
	    break;
	case '-': 
	    tokenBase.push_back(MIN);
	    break;
	case '*': 
	    tokenBase.push_back(MUL);
	    break;
	case '/': 
	    tokenBase.push_back(DIV);
	    break;
	case '%':
	    tokenBase.push_back(MOD);
	    break;
	case '^':
	    tokenBase.push_back(POW);
	    break;
	case ROOT:
	case SIN:
	case TAN:
	case COT:
	case COS:
	case CSC:
	case SEC:
	case LOG:
	case LN:
	case LOG10:
	case ABS:
	case MINF:
	case MAXF:
	case IRAND:
	case FRAND:
	case MOD:
	case FLOOR:
	case SIGN:
	    tokenBase.push_back(t);
	    break;
	case NUM:
	    tokenBase.push_back(t);
	    numBase.push_back(iNumToken);
	    break;
	case ID:  
	    tokenBase.push_back(t);
	    tokenBase.push_back(tval); break;
	default:
	    error();
	    Clear();
	    
	    return;
    }
}
