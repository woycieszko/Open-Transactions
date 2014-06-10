/* See other files here for the LICENCE that applies here. */
/* See header file .hpp for info */

#include "cmd.hpp"

#include "lib_common2.hpp"
#include "ccolor.hpp"

namespace nOT {
namespace nNewcli {

INJECT_OT_COMMON_USING_NAMESPACE_COMMON_2; // <=== namespaces

using namespace nUse;

// ========================================================================================================================

class cCmdParser_pimpl {
	friend class cCmdParser;

	typedef map< cCmdName , shared_ptr<cCmdFormat> >::value_type tTreePair; // type of element (pair) in tree-map. TODO: will be not needed in C+11 map emplace

	private:
		map< cCmdName , shared_ptr<cCmdFormat> > mTree;
};

cCmdParser::cCmdParser() 
: mI( new cCmdParser_pimpl )
{ }

cCmdParser::~cCmdParser() { } // let's instantize default destr in all TUs so compilation will fail (without this line) on unique_ptr on not-complete types trololo - B. Stroustrup

cCmdExecutable::tExitCode Execute1( shared_ptr<cCmdData> , nUse::cUseOT ) {
	_mark("***Execute1***");
	return cCmdExecutable::sSuccess;
}

void cCmdParser::AddFormat( const cCmdName &name, shared_ptr<cCmdFormat> format ) {
	mI->mTree.insert( cCmdParser_pimpl::tTreePair ( name , format ) );
	_info("Add format for command name (" << (string)name << "), now size=" << mI->mTree.size() << " new format is: ");
	// format->Debug();
}

void cCmdParser::PrintUsage() {
	auto & out = cerr;
	out << endl;
	using namespace zkr;
	for(auto element : mI->mTree) {
		string name = element.first;
		shared_ptr<cCmdFormat> format = element.second;
		out << cc::fore::console << "  ot " << cc::fore::green << name << cc::fore::console	<< " " ;
		format->PrintUsageShort(out);
		out << endl;
	}
	out << cc::console;
	out << endl;
}

void cCmdParser::AddFormat(
			const string &name, 
			const vector<cParamInfo> &var,
			const vector<cParamInfo> &varExt,
			const map<string, cParamInfo> &opt,
			const cCmdExecutable::tFunc &exec)
{
	auto format = std::make_shared< cCmdFormat >( cCmdExecutable(exec), var, varExt, opt );
	AddFormat(name, format);
}

void cCmdParser::Init() {
	_mark("Init tree");

	cParamInfo pNym( "nym", "nym existing on a server",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Nym validation");
				return use.NymCheckIfExists(data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Nym hinting");
			return use.NymGetAllNames();
		}
	);

	cParamInfo pNymMy( "nym-my", "one of my own nyms",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Nym validation");
				return use.NymCheckIfExists(data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Nym hinting");
			return use.NymGetAllNames();
		}
	);

	cParamInfo pNymTo = pNym << cParamInfo("nym-to","nym of recipient that exists on a server"); // TODO suggest not the same nym as was used already before
	cParamInfo pNymFrom = pNymMy << cParamInfo("nym-from", "one of your nyms, as the sender");

	cParamInfo pNymNewName( "nym-new", "alias name that will be created",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Nym name validation");
				return true; // Takes all input TODO check if Nym with tis name exists
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Nym name hinting");
			return vector<string> {}; // No hinting for new Nym name
		}
	);
	// ot send cash

	cParamInfo pAccount( "account", "account existing on a server",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Account validation");
				return use.AccountCheckIfExists(data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Account hinting");
			return use.AccountGetAllNames();
		}
	);

	cParamInfo pAccountMy( "account", "on of my accounts",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Account validation");
				return use.AccountCheckIfExists(data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Account hinting");
			return use.AccountGetAllNames();
		}
	);
	cParamInfo pAccountTo = pAccount << cParamInfo("account-to", "account that exists on a server"); // TODO suggest not the same account as was used already before
	cParamInfo pAccountFrom = pAccountMy << cParamInfo("account-from", "one of your accounts, as the outgoing account");

	cParamInfo pAccountNewName( "account-new", "a new account to be created",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Account name validation");
				return true; // Takes all input TODO check if Account with this name exists
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Account name hinting");
			return vector<string> {}; // No hinting for new Nym name
		}
	);


	cParamInfo pAsset( "asset", "asset that exists on a server",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Asset validation");
				return use.AssetCheckIfExists(data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Asset hinting");
			return use.AssetGetAllNames();
		}
	);

	cParamInfo pServer( "server", "identifier of existing server",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Server validation");
				return use.ServerCheckIfExists(data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Server hinting");
			return use.ServerGetAllNames();
		}
	);

	cParamInfo pOnceInt( "int", "integer number",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			// TODO check if is any integer
			// TODO check if not present in data
			return true;
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "-1", "0", "1", "2", "100" };
		}
	);

	cParamInfo pAmount( "int64_t", "Amount of asset",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			// TODO check if is any integer
			// TODO check if can send that amount
			return true;
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> {"1", "10", "100" };
		}
	);

	cParamInfo pSubject( "subject", "the subject",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			return true;
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "hello","hi","test","subject" };
		}
	);

	cParamInfo pBool( "yes-no", "true-false",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			return true; // option's value should be null
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "" }; // this should be empty option, let's continue
		}
	);

	cParamInfo pText( "text", "text",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			return true;
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "" }; // this should be empty option, let's continue
		}
	);

	// sendmoney alice gold 1000
	// sendmsgto alice hi    --addmoney 1000 --addmoney 2000  
	//           arg=1 arg=2           arg=3           arg=4
	// TODO 

	cParamInfo pMsgInIndex( "msg-index-inbox", "index of message in our inbox",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			const int nr = curr_word_ix+1;
			return use.MsgInCheckIndex(data.Var(nr-1), std::stoi( data.Var(nr)) ); //TODO check if integer
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "" }; //TODO hinting function for msg index
		}
	);

	cParamInfo pMsgOutIndex( "msg-index-outbox", "index of message in our outbox",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			const int nr = curr_word_ix+1;
			return use.MsgOutCheckIndex(data.Var(nr-1), std::stoi( data.Var(nr)) ); //TODO check if integer
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "" }; //TODO hinting function for msg index
		}
	);

	// ===========================================================================

	using namespace nOper; // vector + is available inside lambdas
	using std::stoi;

	// types used in lambda header:
	typedef shared_ptr<cCmdData> tData;
	typedef nUse::cUseOT & tUse;
	typedef cCmdExecutable::tExitCode tExit;

	auto & pFrom = pNymFrom;
	auto & pTo = pNymTo;
	auto & pSubj = pSubject;
	auto & pMsg = pSubject; // TODO
	auto & pInt = pOnceInt;

	#define LAMBDA [] (tData d, tUse U) -> tExit

	//======== ot msg ========

	auto this_shared_do_not_use = shared_from_this(); // make_shared<cCmdProcessing>(this);
	weak_ptr<cCmdParser> this_weak( this_shared_do_not_use );

	AddFormat("help", {}, {}, {},
		[this_weak] (tData d, tUse U) -> tExit { auto &D=*d; 
			shared_ptr<cCmdParser> this_lock( this_weak );
			this_lock->PrintUsage();
			return true;
		} );

	AddFormat("msg ls", {}, {pNym}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.MsgDisplayForNym( D.v(1, U.NymGetName(U.NymGetDefault())), D.has("--dryrun") ); } );

	AddFormat("msg sendfrom", {pFrom, pTo}, {pMsg, pSubj}, { {"--dryrun",pBool} , {"--cc",pNym} , {"--bcc",pNym} , {"--prio",pInt} },
		LAMBDA { auto &D=*d; return U.MsgSend(D.V(1), D.V(2) + D.o("--cc") , D.v(3), D.v(4,"nosubject"), stoi(D.o1("--prio","0")), D.has("--dryrun")); }	);

	AddFormat("msg sendto", {pTo}, {pMsg, pSubj}, { {"--dryrun",pBool} , {"--cc",pNym} , {"--bcc",pNym} , {"--prio",pInt} },
		LAMBDA { auto &D=*d; return U.MsgSend(U.NymGetName(U.NymGetDefault()), D.V(1) + D.o("--cc"), D.v(2), D.v(3,"nosubject"), stoi(D.o1("--prio","0")), D.has("--dryrun")); }	);

	AddFormat("msg rm", {pNym, pOnceInt}, {}, { {"--dryrun", pBool} /*{"--all", pBool}*/ }, // FIXME proper handle option without parameter!
		LAMBDA { auto &D=*d; return U.MsgInRemoveByIndex(D.V(1), stoi(D.V(2)), D.has("--dryrun"));} );

	AddFormat("msg rm-out", {pNym, pOnceInt}, {}, { {"--dryrun", pBool} /*{"--all", , pBool}*/ }, // FIXME proper handle option without parameter!
		LAMBDA { auto &D=*d; return U.MsgOutRemoveByIndex(D.V(1), stoi(D.V(2)), D.has("--dryrun")); } );

	//======== ot nym ========

	AddFormat("nym check", {pNym}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.NymCheck( D.V(1), D.has("--dryrun") ); } );

	AddFormat("nym info", {pNym}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.NymDisplayInfo( D.V(1), D.has("--dryrun") ); } );

	AddFormat("nym register", {pNym}, {pServer}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.NymRegister( D.V(1), D.v(2, U.ServerGetName(U.ServerGetDefault())), D.has("--dryrun") ); } );

	AddFormat("nym rm", {pNym}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.NymRemove( D.V(1), D.has("--dryrun") ); } );

	AddFormat("nym new", {pNymNewName}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.NymCreate( D.V(1), D.has("--dryrun") ); } );

	AddFormat("nym set-default", {pNym}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.NymSetDefault( D.V(1), D.has("--dryrun") ); } );

	AddFormat("nym refresh", {}, {pNym}, { {"--dryrun", pBool}, {"--all", pBool}},
		LAMBDA { auto &D=*d; return U.NymRefresh( D.v(1, U.NymGetName( U.NymGetDefault() ) ), D.has("--all"), D.has("--dryrun") ); } );

	AddFormat("nym ls", {}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.NymDisplayAllNames( D.has("--dryrun") ); } );

	AddFormat("nym rename", {pNymMy, pNymNewName}, {}, { {"--dryrun", pBool} },
			LAMBDA { auto &D=*d; return U.NymRename(D.V(1), D.V(2), D.has("--dryrun") ); } );

	//======== ot account ========

	AddFormat("account new", {pAsset, pAccountNewName}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.AccountCreate( D.V(1), D.V(2), D.has("--dryrun") ); } );

	AddFormat("account refresh", {}, {pAccount}, { {"--dryrun", pBool}, {"--all", pBool}},
		LAMBDA { auto &D=*d; return U.AccountRefresh( D.v(1, U.AccountGetName(U.AccountGetDefault())), D.has("--all"), D.has("--dryrun") ); } );

	AddFormat("account set-default", {pAccount}, {}, { {"--dryrun", pBool}},
		LAMBDA { auto &D=*d; return U.AccountSetDefault( D.V(1), D.has("--dryrun") ); } );

	AddFormat("account rm", {pAccount}, {}, { {"--dryrun", pBool}},
		LAMBDA { auto &D=*d; return U.AccountRemove( D.V(1), D.has("--dryrun") ); } );

	AddFormat("account ls", {}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.AccountDisplayAllNames( D.has("--dryrun") ); } );

	AddFormat("account rename", {pAccount, pAccountNewName}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.AccountRename(D.V(1), D.V(2), D.has("--dryrun") ); } );

	AddFormat("account transferfrom", {pAccountFrom, pAccountTo, pAmount}, {pText}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.AccountTransfer(D.V(1), D.V(2), stoi( D.V(3) ), D.v(4), D.has("--dryrun") ); } );

	AddFormat("account transferto", {pAccountTo, pAmount}, {pText}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.AccountTransfer(U.AccountGetName(U.AccountGetDefault()), D.V(1), stoi( D.V(2) ), D.v(3), D.has("--dryrun") ); } );

	//======== ot asset ========

	AddFormat("asset ls", {}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.AssetDisplayAllNames( D.has("--dryrun") ); } );

	AddFormat("asset issue", {}, {pServer, pNym}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.AssetIssue(	D.v(1, U.ServerGetName( U.ServerGetDefault())),
																							D.v(2, U.NymGetName( U.NymGetDefault())),
																							D.has("--dryrun") ); } );

	AddFormat("asset new", {pNym}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.AssetNew(D.V(1), D.has("--dryrun") ); } );

	//======== ot server ========

	AddFormat("server ls", {}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.ServerDisplayAllNames(D.has("--dryrun") ); } );

	AddFormat("server add", {}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.ServerAdd(D.has("--dryrun") ); } );

	AddFormat("server new", {}, {pNymMy}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.ServerCreate(D.v(1, U.NymGetName( U.NymGetDefault())), D.has("--dryrun") ); } );

	AddFormat("server rm", {pServer}, {}, { {"--dryrun", pBool} },
		LAMBDA { auto &D=*d; return U.ServerRemove(D.V(1), D.has("--dryrun") ); } );

	AddFormat("server set-default", {pServer}, {}, { {"--dryrun", pBool}},
		LAMBDA { auto &D=*d; return U.ServerSetDefault( D.V(1), D.has("--dryrun") ); } );

	//======== ot text ========

	AddFormat("text encode", {}, {pText}, { {"--dryrun", pBool} },
			LAMBDA { auto &D=*d; return U.TextEncode(D.v(1, ""), D.has("--dryrun") ); } );

	AddFormat("text decode", {}, {pText}, { {"--dryrun", pBool} },
			LAMBDA { auto &D=*d; return U.TextDecode(D.v(1, ""), D.has("--dryrun") ); } );

	AddFormat("text encrypt", {pNymTo}, {pText}, { {"--dryrun", pBool} },
			LAMBDA { auto &D=*d; return U.TextEncrypt(D.V(1), D.v(2, ""), D.has("--dryrun") ); } );

	AddFormat("text decrypt", {pNymMy}, {pText}, { {"--dryrun", pBool} },
			LAMBDA { auto &D=*d; return U.TextDecrypt(D.V(1), D.v(2, ""), D.has("--dryrun") ); } );

	//mI->tree.emplace( cCmdName("msg send") , msg_send_format );
	
//	mI->tree[ cCmdName("msg send") ] = msg_send_format;

	// msg sendfrom bob alice
	// msg sendfrom bob alice HelloThisIsATest // TODO, other call to OTUse, just pass the message
	// msg sendfrom bob alice "Hello This Is A Test" // TODO, need parser+editline support for quotes

/*	mI->tree[ cCmdName("msg send") ] = cCmdFormat( 
			vector<cParamInfo>{ ARG_STR, ARG_STR, ARG_STR }, map<string,cParamInfo>{{"subject",ARG_STR}}, map<string,cParamInfo>{{"cc",ARG_STR}} ,
	 		[]( nUse::cUseOT &useOt , cCmdData &data ) { 
				string msg=data.arg(3,""); if (0==msg.length()) msg=nUtils::GetMultiline(); 
				useOt->msgSend( data.arg(1), data.arg(2), msg ); }
			);

	
	typedef vector<cParamInfo> vpar;
	typedef map<string,cParamInfo> mopt;

	mI->tree[ cCmdName("msg send") ] = cCmdFormat( useOt::msgList, 
		vpar{ ARG_STR, ARG_STR, ARG_STR },  vpar{},  vopt{{"subject",ARG_STR}},  mopt{{"cc",ARG_STR}, {"bcc",ARG_STR}} );
*/
}

cCmdProcessing cCmdParser::StartProcessing(const vector<string> &words, shared_ptr<nUse::cUseOT> use ) {
	return cCmdProcessing( shared_from_this() , words , use );
}

cCmdProcessing cCmdParser::StartProcessing(const string &words, shared_ptr<nUse::cUseOT> use ) {
	_dbg3("Will split words: [" << words << "]");
	return cCmdProcessing( shared_from_this() , nUtils::SplitString(words) , use );
}

shared_ptr<cCmdFormat> cCmdParser::FindFormat( const cCmdName &name ) 
	throw(cErrParseName)
{
	auto it = mI->mTree.find( name );
	if (it == mI->mTree.end()) {
		throw cErrParseName("No such ot command="+(string)name);
	}
	return it->second;
}

// ========================================================================================================================

cCmdName::cCmdName(const string &name) : mName(name) { }

bool cCmdName::operator<(const cCmdName &other) const { return mName < other.mName; }

cCmdName::operator std::string() const { return mName; }

// ========================================================================================================================

cCmdProcessing::cCmdProcessing(shared_ptr<cCmdParser> parser, vector<string> commandLine, shared_ptr<nUse::cUseOT> use )
: 
mStateParse(tState::never), mStateValidate(tState::never), mStateExecute(tState::never),
mParser(parser), mCommandLine(commandLine), mUse(use)
{ 
	_dbg2("Creating processing of: " << DbgVector(commandLine) << " with use=" << use->DbgName() );
}

cCmdProcessing::~cCmdProcessing() 
{ }

void cCmdProcessing::Validate() { 
	if (mStateValidate != tState::never) { _dbg1("Validation was done already"); return; }
	mStateValidate = tState::failed; // assumed untill succeed below
	try {
		_Validate();
		mStateValidate = tState::succeeded;
		_dbg1("Validation succeeded");
	} catch (const myexception &e) { e.Report(); throw ; } catch (const std::exception &e) { _erro("Exception " << e.what()); throw ; }
}

void cCmdProcessing::_Validate() { 
	if (mStateParse != tState::succeeded) Parse();
	if (mStateParse != tState::succeeded) { _dbg1("Failed to parse."); }

	if (!mData) { _warn("Can not validate - there is no mData"); return; }
	if (!mFormat) { _warn("Can not validate - there is no mData"); return; }

	const auto sizeAll = mData->SizeAllVar();
	_dbg2("Will validate all variables, size=" << sizeAll );
	
	for (size_t nr=1; nr<=sizeAll; ++nr) { // TODO:nrix 
		auto var = mData->Var(nr); // get the var
		const cParamInfo & info = mFormat->GetParamInfo(nr);
		auto func = info.GetFuncValid();
		bool ok = func( *mUse, *mData, nr-1 ); // ***
		if (!ok) { 
			const string err = ToStr("Validation failed at nr=") + ToStr(nr) + " for var=" + ToStr(var); 
			_warn(err);
			throw myexception(err);
		}
	}
}

void cCmdProcessing::Parse() {
	if (mStateParse != tState::never) { _dbg1("Already parsed"); return; }
	mStateParse = tState::failed; // assumed untill succeed below
	try {
		_Parse();
		mStateParse = tState::succeeded;
	} catch (const myexception &e) { e.Report(); throw ; } catch (const std::exception &e) { _erro("Exception " << e.what()); throw ; }
}

void cCmdProcessing::_Parse() {
	int _dbg_ignore=50;

	// mCommandLine = ot, msg, sendfrom, alice, bob, hello
	// mFormat.erase ? // remove old format, we parse something new [doublecheck]

	if (mCommandLine.empty()) {
		_warn("Command for processing is totally empty");
		throw cErrParseSyntax("Command was empty"); // <--- THROW
	}

	if (mCommandLine.at(0) == "help") mCommandLine.insert( mCommandLine.begin() , "ot"); // change "help" to "ot help"

	if (mCommandLine.at(0) != "ot") {
		_warn("Command for processing is mallformed");
	}
	mCommandLine.erase( mCommandLine.begin() ); // delete the first "ot" ***
	// mCommandLine = msg, sendfrom, alice, bob, hello
	_dbg1("Parsing (after erasing ot) : " << DbgVector(mCommandLine) );

	if (mCommandLine.empty()) {
		_warn("Command for processing is empty (after erasing ot)");
		throw cErrParseSyntax("Command was empty (besides prefix)"); // <--- THROW
	}

	_dbg3("Alloc data");  
	mData = std::make_shared<cCmdData>();
	_dbg3("Got new data");

	int phase=0; // 0: cmd name  1:var, 2:varExt  3:opt   9:end
	try {
		const string name = mCommandLine.at(0) 
			+ (   (mCommandLine.size()<=1)  ?  "" : (" " + mCommandLine.at(1))   ); // this is second-word plus space, or nothing
		// "msg send" or "help"
		_dbg3("Name of command is: " << name);
		mFormat = mParser->FindFormat( name );
		_info("Got format for name="<<name);

		// msg send
		// msg ls
		// always 2 words are the command (we assume there are no sub-command)
		const size_t words_count = mCommandLine.size();
		const cCmdFormat & format = * mFormat; // const to be sure to just read from it (we are friends of this class)
		const size_t var_size_normal = format.mVar.size(); // number of the normal (mandatory) part of variables
		const size_t var_size_all = format.mVar.size() + format.mVarExt.size(); // number of the size of all variables (normal + extra)
		_dbg2("Format: size of vars: " << var_size_normal << " normal, and all is: " << var_size_all);
		int pos = 2; // "msg send"

		phase=1;
		const size_t offset_to_var = pos; // skip this many words before we have first var, to conver pos(word number) to var number

		if (phase==1) {
			while (true) { // parse var normal
				const int var_nr = pos - offset_to_var;
				_dbg2("phase="<<phase<<" pos="<<pos<<" var_nr="<<var_nr);
				if (pos >= words_count) { _dbg1("reached END, pos="<<pos);	phase=9; break;	}
				if (var_nr >= var_size_normal) { _dbg1("reached end of var normal, var_nr="<<var_nr); phase=2;	break;	}

				string word = mCommandLine.at(pos);
				_dbg1("phase="<<phase<<" pos="<<pos<<" word="<<word);
				++pos;

				if ( nUtils::CheckIfBegins("\"", word) ) { // TODO review memory access
					_dbg1("Quotes detected in: " + word);
					word.erase(0,1);
					while ( !nUtils::CheckIfEnds("\"", word) ) {
						word += " " + mCommandLine.at(pos);
						++pos;
					}
					word.erase(word.end()-1, word.end()); // ease the closing " of last mCommandLine[..] that is not at end of word
					_dbg1("Quoted word is:"<<word);
				}
				if (nUtils::CheckIfBegins("--", word)) { // --bcc foo
					phase=3; --pos; // this should be re-prased in proper phase
					_dbg1("Got an --option, so jumping to phase="<<phase);
					break; // continue to phase 3 - the options
				}

				_dbg1("adding var "<<word);  mData->mVar.push_back( word ); 
			}
		} // parse var phase 1

		if (phase==2) {
			while (true) { // parse var normal
				const int var_nr = pos - offset_to_var;
				_dbg2("phase="<<phase<<" pos="<<pos<<" var_nr="<<var_nr);
				if (pos >= words_count) { _dbg1("reached END, pos="<<pos);	phase=9; break;	}
				if (var_nr >= var_size_all) { _dbg1("reached end of var ALL, var_nr="<<var_nr); phase=3;	break;	}

				string word = mCommandLine.at(pos);
				_dbg1("phase="<<phase<<" pos="<<pos<<" word="<<word);
				++pos;

				if ( nUtils::CheckIfBegins("\"", word) ) { // TODO review memory access
					_dbg1("Quotes detected in: " + word);
					word.erase(0,1);
					while ( !nUtils::CheckIfEnds("\"", word) ) {
						word += " " + mCommandLine.at(pos);
						++pos;
					}
					word.erase(word.end()-1, word.end()); // ease the closing " of last mCommandLine[..] that is not at end of word
					_dbg1("Quoted word is:"<<word);
				}
				if (nUtils::CheckIfBegins("--", word)) { // --bcc foo
					phase=3; --pos; // this should be re-prased in proper phase
					_dbg1("Got an --option, so jumping to phase="<<phase);
					break; // continue to phase 3 - the options
				}

				_dbg1("adding var ext "<<word);  mData->mVarExt.push_back( word ); 
			}
		} // phase 2

		if (phase==3) {
			string prev_name="";  bool inside_opt=false; // are we now in middle of --option ?  curr_name is the opt name like "--cc"
			while (true) { // parse options
				if (pos >= words_count) { _dbg1("reached END, pos="<<pos);	phase=9; break;	}

				string word = mCommandLine.at(pos);
				_dbg1("phase="<<phase<<" pos="<<pos<<" word="<<word);
				++pos;

				bool is_newopt =  nUtils::CheckIfBegins("--", word); // word is opt name like "--cc"

				if (is_newopt) { // some new option like --private or --cc
					if (inside_opt) { // finish the previos option, that didn't got a value then.  --fast [--private]
						inside_opt=false;
						mData->AddOpt(prev_name , "");
						_dbg1("got option "<<prev_name<<" (empty)");
					}
					inside_opt=true; prev_name=word; // we now started the new option (and next iteration will finish it)
					_dbg3("started new option: prev_name="<<prev_name);
				}
				else { // not an --option, so should be a value to finish previous one
					if (inside_opt) { // we are in middle of option, now we have the argment that ends it: --cc [alice]
						string value=word; // like "alice"
						inside_opt=false;
						mData->AddOpt(prev_name , value);
						_dbg1("got option "<<prev_name<<" with value="<<value);
					}
					else { // we have a word like "bob", but we are not in middle of an option - syntax error
						throw cErrParseSyntax("Expected an --option here, but got a word=" + ToStr(word) + " at pos=" + ToStr(pos));
					}
				}
			} // all words
			if (inside_opt) { // finish the previos LAST option, that didn't got a value then.  --fast [--private] (END)
				inside_opt=false;
				mData->AddOpt(prev_name , "");
				_dbg1("got option "<<prev_name<<" (empty) - on end");
			}
		} // phase 3

		_note("mVar parsed:    " + DbgVector(mData->mVar));
		_note("mVarExt parsed: " + DbgVector(mData->mVarExt));
		_note("mOption parsed  " + DbgMap(mData->mOption));
 
	} 
	catch (cErrParse &e) {
		_warn("Command can not be parsed " << e.what());
		throw ;
	}
	catch (std::exception &e) {
		_erro("Internal error in parser code " << e.what() << " while parsing:" << DbgVector( mCommandLine ) );
		throw ;
	}
	catch (myexception &e) {
		_erro("Internal error in parser code " << e.what() << " while parsing:" << DbgVector( mCommandLine ) );
		throw ;
	}
}

vector<string> cCmdProcessing::UseComplete() {
	vector<string> ret;
	return ret;
}

void cCmdProcessing::UseExecute() { // TODO write as a template for all the 3 wrappres that set state ??
	if (mStateParse != tState::succeeded) Parse();
	if (mStateValidate != tState::succeeded) Validate();
	if (mStateParse != tState::succeeded) { _dbg1("Failed to parse."); }
	if (mStateValidate != tState::succeeded) { _dbg1("Failed to validate."); }

	if (mStateExecute != tState::never) { _dbg1("Exec was done already"); return; }
	mStateExecute = tState::failed; // assumed untill succeed below
	try {
		_UseExecute();
		mStateExecute = tState::succeeded;
	} catch (const myexception &e) { e.Report(); throw ; } catch (const std::exception &e) { _erro("Exception " << e.what()); throw ; }
}

void cCmdProcessing::_UseExecute() {
	if (!mFormat) { _warn("Can not execute this command - mFormat is empty"); return; }
	cCmdExecutable exec = mFormat->getExec();
	exec( mData , *mUse ); 
}

// ========================================================================================================================

cParamInfo::cParamInfo(const string &name, const string &descr, tFuncValid valid, tFuncHint hint) 
	: mName(name), mDescr(descr), funcValid(valid), funcHint(hint)
{ }

cParamInfo::cParamInfo(const string &name, const string &descr)
	: mName(name), mDescr(descr)
{ }

cParamInfo cParamInfo::operator<<(const cParamInfo &B) const {
	cParamInfo A = *this;
	A.mName = B.mName;
	A.mDescr = B.mDescr;
	if (B.funcValid) A.funcValid = B.funcValid;
	if (B.funcHint) A.funcHint = B.funcHint;
	return A;
}

// ========================================================================================================================

// cCmdFormat::cCmdFormat(cCmdExecutable exec, tVar var, tVar varExt, tOption opt) 
cCmdFormat::cCmdFormat(const cCmdExecutable &exec, const tVar &var, const tVar &varExt, const tOption &opt)
	:	mExec(exec), mVar(var), mVarExt(varExt), mOption(opt)
{
	_dbg1("Created new format");
}

cParamInfo cCmdFormat::GetParamInfo(int nr) const {
	// similar to cCmdData::VarAccess()
	if (nr <= 0) throw myexception("Illegal number for var, nr="+ToStr(nr));
	const int ix = nr - 1;
	if (ix >= mVar.size()) { // then this is an extra argument
		const int ix_ext = ix - mVar.size();
		if (ix_ext >= mVarExt.size()) { // then this var number does not exist - out of range
			throw myexception("Missing argument: out of range number for var, nr="+ToStr(nr)+" ix="+ToStr(ix)+" ix_ext="+ToStr(ix_ext)+" vs size="+ToStr(mVarExt.size()));
		}
		return mVarExt.at(ix_ext);
	}
	return mVar.at(ix);
}

size_t cCmdFormat::SizeAllVar() const { // return size of required mVar + optional mVarExt
	return mVar.size() + mVarExt.size();
}

cCmdExecutable cCmdFormat::getExec() const {
	return mExec;
}

void cCmdFormat::Debug() const {
	_info("Format at " << (void*)this );
	_info(".. mVar size=" << mVar.size());
	_info(".. mVarExt size=" << mVarExt.size());
	_info(".. mOption size=" << mOption.size());
}

void cCmdFormat::PrintUsageShort(ostream &out) const {
	using namespace zkr;
	bool written=false;
	{ 
		out << cc::fore::lightyellow;
		size_t nr=0;  for(auto var : mVar) { if (nr) out<<" ";  out << (string)var;  ++nr; written=true; }
	}
	if (written) out<<" ";

	written=false;
	{ 
		out << cc::fore::lightcyan;
		size_t nr=0;  for(auto var : mVarExt) { if (nr) out<<" ";  out << '[' << (string)var <<']';  ++nr; written=true; }
	}
	if (written) out<<" ";

	written=false;
	{ 
		out << cc::fore::lightblue;
		size_t nr=0;  
		for(auto opt : mOption) { if (nr) out<<" ";  
			string name = opt.first;
			out << "[" << name <<']';  
			++nr; 
			written=true;
		}		
	}
	if (written) out<<" ";
	out << cc::fore::console;
}

// ========================================================================================================================

cCmdExecutable::tExitCode cCmdExecutable::operator()( shared_ptr<cCmdData> data, nUse::cUseOT & useOt) {
	_info("Executing function");
	int ret = mFunc( data , useOt );
	_info("Execution ret="<<ret);
	return ret;
}

cCmdExecutable::cCmdExecutable(tFunc func) : mFunc(func) { }

const cCmdExecutable::tExitCode cCmdExecutable::sSuccess = 0; 

// ========================================================================================================================

size_t cCmdData::SizeAllVar() const { // return size of required mVar + optional mVarExt
	return mVar.size() + mVarExt.size();
}

string cCmdData::VarAccess(int nr, const string &def, bool doThrow) const throw(cErrArgNotFound) { // see [nr] ; if doThrow then will throw on missing var, else returns def
	if (nr <= 0) throw cErrArgIllegal("Illegal number for var, nr="+ToStr(nr)+" (1,2,3... is expected)");
	const int ix = nr - 1;
	if (ix >= mVar.size()) { // then this is an extra argument
		const int ix_ext = ix - mVar.size();
		if (ix_ext >= mVarExt.size()) { // then this var number does not exist - out of range
			if (doThrow) {
				throw cErrArgMissing("Missing argument: out of range number for var, nr="+ToStr(nr)+" ix="+ToStr(ix)+" ix_ext="+ToStr(ix_ext)+" vs size="+ToStr(mVarExt.size()));
			}
			return def; // just return the default
		}
		return mVarExt.at(ix_ext);
	}
	return mVar.at(ix);
}

void cCmdData::AssertLegalOptName(const string &name) const throw(cErrArgIllegal) {
	if (name.size()<1) throw cErrArgIllegal("option name can not be empty");
	const size_t maxlen=100;
	if (name.size()>maxlen) throw cErrArgIllegal("option name too long, over" + ToStr(maxlen));
	// TODO test [a-zA-Z0-9_.-]*
}

vector<string> cCmdData::OptIf(const string& name) const throw(cErrArgIllegal) {
	AssertLegalOptName(name);
	auto find = mOption.find( name );
	if (find == mOption.end()) { 
		return vector<string>{};
	} 
	return find->second;
}

string cCmdData::Opt1If(const string& name, const string &def) const throw(cErrArgIllegal) { // same but requires the 1st element; therefore we need def argument again
	AssertLegalOptName(name);
	auto find = mOption.find( name );
	if (find == mOption.end()) { 
		return def;
	} 
	const auto &vec = find->second;
	if (vec.size()<1) { _warn("Not normalized opt for name="<<name); return def; }
	return vec.at(0);
}


string cCmdData::VarDef(int nr, const string &def, bool doThrow) const throw(cErrArgIllegal) {
	return VarAccess(nr, def, false);
}

string cCmdData::Var(int nr) const throw(cErrArgNotFound) { // nr: 1,2,3,4 including both arg and argExt
	static string nothing;
	return VarAccess(nr, nothing, true);
}

vector<string> cCmdData::Opt(const string& name) const throw(cErrArgNotFound) {
	AssertLegalOptName(name);
	auto find = mOption.find( name );
	if (find == mOption.end()) { _warn("Map was: [TODO]"); throw cErrArgMissing("Option " + name + " was missing"); } 
	return find->second;
}

string cCmdData::Opt1(const string& name) const throw(cErrArgNotFound) {
	AssertLegalOptName(name);
	auto find = mOption.find( name );
	if (find == mOption.end()) {  throw cErrArgMissing("Option " + name + " was missing"); } 
	const auto &vec = find->second;
	if (vec.size()<1) { _warn("Not normalized opt for name="<<name); throw cErrArgMissing("Option " + name + " was missing (not-normalized empty vector)"); }
	return vec.at(0);
}

bool cCmdData::IsOpt(const string &name) const throw(cErrArgIllegal) {
	AssertLegalOptName(name);
	auto find = mOption.find( name );
	if (find == mOption.end()) { 
		return false; // no such option entry
	} 
	auto &vect = find->second;
	if (vect.size()) {
		return true; // yes, there is an option
	}

	_warn("Not normalized options for name="<<name<<" an empty vector exists there:" << DbgVector(vect));
	return false; // there was a vector for this options but it's empty now (maybe deleted?)
}

void cCmdData::AddOpt(const string &name, const string &value) throw(cErrArgIllegal) { // append an option with value (value can be empty
	_dbg3("adding option ["<<name<<"] with value="<<value);
	auto find = mOption.find( name );
	if (find == mOption.end()) {
		mOption.insert( std::make_pair( name , vector<string>{ value } ) );	
	} else {
		find->second.push_back( value );
	}	
}

// ========================================================================================================================

void _cmd_test( shared_ptr<cUseOT> use ) {
	_mark("TEST TREE");

	shared_ptr<cCmdParser> parser(new cCmdParser);
	parser->Init();

	auto alltest = vector<string>{ ""
	//ot msg --dryrun
	,"ot help"
/*	,"ot msg ls --dryrun"
	,"ot msg ls alice --dryrun"*/
	,"ot msg sendfrom alice bob --prio 1 --dryrun"
/*	,"ot msg sendfrom alice bob --cc eve --cc mark --bcc john --prio 4 --dryrun"
	,"ot msg sendfrom alice bob message subject --cc eve --cc mark --bcc john --prio 4 --dryrun"
	,"ot msg sendto bob hello --cc eve --cc mark --bcc john --prio 4 --dryrun"
	,"ot msg rm alice 0 --dryrun"
	,"ot msg rm-out alice 0 --dryrun"
	//ot msg
//	"ot msg ls"
//	,"ot msg ls alice"
//	,"ot msg sendfrom alice bob --prio 1"
//	,"ot msg sendfrom alice bob --cc eve --cc mark --bcc john --prio 4"
//	,"ot msg sendfrom alice bob --cc eve --cc mark --bcc john --prio 4"
//	,"ot msg sendto bob hello --cc eve --cc mark --bcc john --prio 4"
//	,"ot msg rm alice 0"
//	,"ot msg rm-out alice 0"

	//ot nym --dryrun
	,"ot nym check alice --dryrun"
	,"ot nym info alice --dryrun"
	,"ot nym register alice --dryrun"
	,"ot nym rm alice --dryrun"
	,"ot nym new alice --dryrun"
	,"ot nym set-default alice --dryrun"
	,"ot nym refresh --dryrun"
	,"ot nym refresh alice --dryrun"
	,"ot nym ls --dryrun"
	//ot nym
//	,"ot nym check alice"
//	,"ot nym info alice"
//	,"ot nym register alice"
//	,"ot nym rm alice"
//	,"ot nym new alice"
//	,"ot nym set-default alice"
//	,"ot nym refresh"
//	,"ot nym refresh alice"
//	,"ot nym ls"
	//ot account --dryrun
	,"ot account new assetname accountname --dryrun"
	,"ot account refresh --dryrun"
	,"ot account refresh accountname --dryrun"
	,"ot account set-default accountname --dryrun"
	,"ot account rm accountname --dryrun"
	,"ot account ls --dryrun"
	,"ot account mv accountname newaccountname --dryrun"
	//ot account
//	,"ot account new assetname accountname"
//	,"ot account refresh"
//	,"ot account refresh accountname"
//	,"ot account set-default accountname"
//	,"ot account rm accountname"
//	,"ot account ls"
//	,"ot account mv accountname newaccountname"
	//ot asset --dryrun
	,"ot asset new assetname --dryrun"
	,"ot asset issue --dryrun"
	,"ot asset ls --dryrun"
	//ot asset
//	,"ot asset new"
//	,"ot asset issue"
//	,"ot asset ls"
	//ot server --dryrun
	,"ot server add --dryrun"
	,"ot server ls --dryrun"
	,"ot server new --dryrun"
	,"ot server rm servername --dryrun"
	,"ot server set-default servername --dryrun"
	//ot server
//	,"ot server add"
//	,"ot server ls"
//	,"ot server new"
//	,"ot server rm servername"
//	,"ot server set-default servername"
	//ot text --dryrun
	,"ot text encode texttoprocess --dryrun"
	,"ot text decode text --dryrun"
	,"ot text encrypt bob text --dryrun"
	,"ot text decrypt text --dryrun"
	//ot text
//	,"ot text encode texttoprocess"
//	,"ot text decode text"
//	,"ot text encrypt bob text"
//	,"ot text decrypt text"
*/
	};
	for (auto cmd : alltest) {
		try {
			if (!cmd.length()) continue;
			_mark("====== Testing command: " << cmd );
			auto processing = parser->StartProcessing(cmd, use);
			processing.UseExecute();
		} catch (const myexception &e) { e.Report(); throw ; } catch (const std::exception &e) { _erro("Exception " << e.what()); throw ; }
	}

}

void cmd_test( shared_ptr<cUseOT> use ) {
	try {
		_cmd_test(use);
	} catch (const myexception &e) { e.Report(); throw ; } catch (const std::exception &e) { _erro("Exception " << e.what()); throw ; }
}

}; // namespace nNewcli
}; // namespace OT


