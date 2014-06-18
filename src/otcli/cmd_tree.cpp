
#include "cmd.hpp"
#include "cmd_detail.hpp"

#include "lib_common2.hpp"
#include "ccolor.hpp"

namespace nOT {
namespace nNewcli {

INJECT_OT_COMMON_USING_NAMESPACE_COMMON_2; // <=== namespaces

using namespace nUse;

void cCmdParser::_AddFormat( const cCmdName &name, shared_ptr<cCmdFormat> format ) {
	mI->mTree.insert( cCmdParser_pimpl::tTreePair ( name , format ) );
	_info("Add format for command name (" << (string)name << "), now size=" << mI->mTree.size() << " new format is: ");
	// format->Debug();
}

void cCmdParser::AddFormat(
			const string &name, 
			const vector<cParamInfo> &var,
			const vector<cParamInfo> &varExt,
			const map<string, cParamInfo> &opt,
			const cCmdExecutable::tFunc &exec)
{
	using namespace nOper;
	auto format = std::make_shared< cCmdFormat >( cCmdExecutable(exec), var, varExt, opt + mI->mCommonOpt );
	_AddFormat(name, format);
}


void cCmdParser::Init() {
	_mark("Init tree");

// TODO hinting for ID?
	cParamInfo pNym( "nym", "nym existing on a server",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Nym validation");
			return use.CheckIfExists(nUtils::eSubjectType::User, data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Nym hinting");
			return use.NymGetAllNames();
		}
	);

	cParamInfo pNymMy( "nym-my", "one of my own nyms",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Nym validation");
				return use.CheckIfExists(nUtils::eSubjectType::User, data.Var(curr_word_ix + 1));
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
				return use.CheckIfExists(nUtils::eSubjectType::Account, data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Account hinting");
			return use.AccountGetAllNames();
		}
	);

	cParamInfo pAccountMy( "account", "on of my accounts",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Account validation");
				return use.CheckIfExists(nUtils::eSubjectType::Account, data.Var(curr_word_ix + 1));
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
				return use.CheckIfExists(nUtils::eSubjectType::Asset, data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Asset hinting");
			return use.AssetGetAllNames();
		}
	);

	cParamInfo pServer( "server", "identifier of existing server",
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Server validation");
				return use.CheckIfExists(nUtils::eSubjectType::Server, data.Var(curr_word_ix + 1));
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

	cParamInfo pAmount( "amount", "Amount of asset",
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

	cParamInfo pBoolBoring( "yes-no", "true-false",
		pBool.funcValid , pBool.funcHint
		, cParamInfo::eFlags::isBoring 
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
	// COMMON OPTIONS

	mI->mCommonOpt.clear();
	mI->mCommonOpt.insert( {"--dryrun", pBoolBoring} );

	// ===========================================================================

	using namespace nOper; // vector + is available inside lambdas
	using std::stoi;

	// types used in lambda header:
	typedef shared_ptr<cCmdData> tData;
	typedef nUse::cUseOT & tUse;
	typedef cCmdExecutable::tExitCode tExit;
	#define LAMBDA [] (tData d, tUse U) -> tExit

	auto & pFrom = pNymFrom;
	auto & pTo = pNymTo;
	auto & pSubj = pSubject;
	auto & pMsg = pSubject; // TODO
	auto & pInt = pOnceInt;

	auto this_shared_do_not_use = shared_from_this(); // make_shared<cCmdProcessing>(this);
	weak_ptr<cCmdParser> this_weak( this_shared_do_not_use );

	//======== special, test, developer: ========

	AddFormat("help", {}, {}, {},
		[this_weak] (tData d, tUse U) -> tExit { auto &D=*d; 
			shared_ptr<cCmdParser> this_lock( this_weak );
			this_lock->PrintUsage();
			return true;
		} );

	AddFormat("x", {}, {}, {},
		LAMBDA { auto &D=*d; auto Utmp = make_shared<cUseOT>( U ); _cmd_test(Utmp); return true; } );

	AddFormat("test", {}, {}, {},
		LAMBDA { auto &D=*d; auto Utmp = make_shared<cUseOT>( U ); _cmd_test(Utmp); return true; } );

	AddFormat("test tab", {}, {}, {},
		LAMBDA { auto &D=*d; auto Utmp = make_shared<cUseOT>( U ); _cmd_test_completion(Utmp); return true; } );

	AddFormat("test tree", {}, {}, {},
		LAMBDA { auto &D=*d; auto Utmp = make_shared<cUseOT>( U ); _cmd_test_tree(Utmp); return true; } );

	//======== ot account ========

	AddFormat("account", {}, {}, {},
		LAMBDA { auto &D=*d; return U.DisplayDefaultID(nUtils::eSubjectType::Account, D.has("--dryrun") ); } ); //TODO

	AddFormat("account new", {pAsset, pAccountNewName}, {}, {},
		LAMBDA { auto &D=*d; return U.AccountCreate( D.V(1), D.V(2), D.has("--dryrun") ); } );

	AddFormat("account refresh", {}, {pAccount}, { {"--all", pBool } },
		LAMBDA { auto &D=*d; return U.AccountRefresh( D.v(1, U.AccountGetName(U.AccountGetDefault())), D.has("--all"), D.has("--dryrun") ); } );

	AddFormat("account set-default", {pAccount}, {}, {},
		LAMBDA { auto &D=*d; return U.AccountSetDefault( D.V(1), D.has("--dryrun") ); } );

	AddFormat("account rm", {pAccount}, {}, {},
		LAMBDA { auto &D=*d; return U.AccountRemove( D.V(1), D.has("--dryrun") ); } );

	AddFormat("account ls", {}, {}, {},
		LAMBDA { auto &D=*d; return U.AccountDisplayAll( D.has("--dryrun") ); } );

	AddFormat("account show", {pAccount}, {}, {},
		LAMBDA { auto &D=*d; return U.AccountDisplay( D.V(1), D.has("--dryrun") ); } );

	AddFormat("account rename", {pAccount, pAccountNewName}, {}, {},
		LAMBDA { auto &D=*d; return U.AccountRename(D.V(1), D.V(2), D.has("--dryrun") ); } );

	AddFormat("account transfer-from", {pAccountFrom, pAccountTo, pAmount}, {pText}, {},
		LAMBDA { auto &D=*d; return U.AccountTransfer(D.V(1), D.V(2), stoi( D.V(3) ), D.v(4), D.has("--dryrun") ); } );

	AddFormat("account transfer-to", {pAccountTo, pAmount}, {pText}, {},
		LAMBDA { auto &D=*d; return U.AccountTransfer(U.AccountGetName(U.AccountGetDefault()), D.V(1), stoi( D.V(2) ), D.v(3), D.has("--dryrun") ); } );

	//======== ot account-in ========

	AddFormat("account-in ls", {}, {pAccountMy}, {},
		LAMBDA { auto &D=*d; return U.AccountInDisplay(D.v(1, U.AccountGetName(U.AccountGetDefault())), D.has("--dryrun") ); } );

	//======== ot asset ========

	AddFormat("asset ls", {}, {}, {},
		LAMBDA { auto &D=*d; return U.AssetDisplayAll( D.has("--dryrun") ); } );

	AddFormat("asset issue", {}, {pServer, pNym}, {},
		LAMBDA { auto &D=*d; return U.AssetIssue(	D.v(1, U.ServerGetName( U.ServerGetDefault())),
																							D.v(2, U.NymGetName( U.NymGetDefault())),
																							D.has("--dryrun") ); } );

	AddFormat("asset new", {pNym}, {}, {},
		LAMBDA { auto &D=*d; return U.AssetNew(D.V(1), D.has("--dryrun") ); } );

	AddFormat("asset rm", {pNym}, {}, {},
		LAMBDA { auto &D=*d; return U.AssetRemove(D.V(1), D.has("--dryrun") ); } );


	//======== ot msg ========

	AddFormat("msg ls", {}, {pNym}, {},
		LAMBDA { auto &D=*d; return U.MsgDisplayForNym( D.v(1, U.NymGetName(U.NymGetDefault())), D.has("--dryrun") ); } );

	AddFormat("msg send-from", {pFrom, pTo}, {pSubj, pMsg}, { {"--cc",pNym} , {"--bcc",pNym} , {"--prio",pInt} },
		LAMBDA { auto &D=*d; return U.MsgSend(D.V(1), D.V(2) + D.o("--cc") , D.v(3), D.v(4,"nosubject"), stoi(D.o1("--prio","0")), D.has("--dryrun")); }	);

	AddFormat("msg send-to", {pTo}, {pSubj, pMsg}, { {"--cc",pNym} , {"--bcc",pNym} , {"--prio",pInt} },
		LAMBDA { auto &D=*d; return U.MsgSend(U.NymGetName(U.NymGetDefault()), D.V(1) + D.o("--cc"), D.v(2,"nosubject"), D.v(3), stoi(D.o1("--prio","0")), D.has("--dryrun")); }	);

	AddFormat("msg rm", {pNym, pOnceInt}, {}, {/*{"--all", pBool}*/ }, // FIXME proper handle option without parameter!
		LAMBDA { auto &D=*d; return U.MsgInRemoveByIndex(D.V(1), stoi(D.V(2)), D.has("--dryrun"));} );

	AddFormat("msg rm-out", {pNym, pOnceInt}, {}, {/*{"--all", , pBool}*/ }, // FIXME proper handle option without parameter!
		LAMBDA { auto &D=*d; return U.MsgOutRemoveByIndex(D.V(1), stoi(D.V(2)), D.has("--dryrun")); } );

	//======== ot nym ========

	AddFormat("nym check", {pNym}, {}, {},
		LAMBDA { auto &D=*d; return U.NymCheck( D.V(1), D.has("--dryrun") ); } );

	AddFormat("nym info", {pNym}, {}, {},
		LAMBDA { auto &D=*d; return U.NymDisplayInfo( D.V(1), D.has("--dryrun") ); } );

	AddFormat("nym register", {pNym}, {pServer}, {} ,
	LAMBDA { auto &D=*d; return U.NymRegister( D.V(1), D.v(2, U.ServerGetName(U.ServerGetDefault())), D.has("--dryrun") ); } );

	AddFormat("nym rm", {pNym}, {}, { {} },
		LAMBDA { auto &D=*d; return U.NymRemove( D.V(1), D.has("--dryrun") ); } );

	AddFormat("nym new", {pNymNewName}, {}, { {} },
		LAMBDA { auto &D=*d; return U.NymCreate( D.V(1), D.has("--dryrun") ); } );

	AddFormat("nym set-default", {pNym}, {}, { {} },
		LAMBDA { auto &D=*d; return U.NymSetDefault( D.V(1), D.has("--dryrun") ); } );

	AddFormat("nym refresh", {}, {pNym}, { {}, {"--all", pBool}},
		LAMBDA { auto &D=*d; return U.NymRefresh( D.v(1, U.NymGetName( U.NymGetDefault() ) ), D.has("--all"), D.has("--dryrun") ); } );

	AddFormat("nym ls", {}, {}, {},
		LAMBDA { auto &D=*d; return U.NymDisplayAll( D.has("--dryrun") ); } );

	AddFormat("nym rename", {pNymMy, pNymNewName}, {}, {},
			LAMBDA { auto &D=*d; return U.NymRename(D.V(1), D.V(2), D.has("--dryrun") ); } );


	//======== ot server ========

	AddFormat("server ls", {}, {}, {},
		LAMBDA { auto &D=*d; return U.ServerDisplayAll(D.has("--dryrun") ); } );

	AddFormat("server add", {}, {}, {},
		LAMBDA { auto &D=*d; return U.ServerAdd(D.has("--dryrun") ); } );

	AddFormat("server new", {}, {pNymMy}, {},
		LAMBDA { auto &D=*d; return U.ServerCreate(D.v(1, U.NymGetName( U.NymGetDefault())), D.has("--dryrun") ); } );

	AddFormat("server rm", {pServer}, {}, {},
		LAMBDA { auto &D=*d; return U.ServerRemove(D.V(1), D.has("--dryrun") ); } );

	AddFormat("server set-default", {pServer}, {}, {},
		LAMBDA { auto &D=*d; return U.ServerSetDefault( D.V(1), D.has("--dryrun") ); } );

	//======== ot text ========

	AddFormat("text encode", {}, {pText}, {},
			LAMBDA { auto &D=*d; return U.TextEncode(D.v(1, ""), D.has("--dryrun") ); } );

	AddFormat("text decode", {}, {pText}, {},
			LAMBDA { auto &D=*d; return U.TextDecode(D.v(1, ""), D.has("--dryrun") ); } );

	AddFormat("text encrypt", {pNymTo}, {pText}, {},
			LAMBDA { auto &D=*d; return U.TextEncrypt(D.V(1), D.v(2, ""), D.has("--dryrun") ); } );

	AddFormat("text decrypt", {pNymMy}, {pText}, {},
			LAMBDA { auto &D=*d; return U.TextDecrypt(D.V(1), D.v(2, ""), D.has("--dryrun") ); } );

	//mI->tree.emplace( cCmdName("msg send") , msg_send_format );
	
//	mI->tree[ cCmdName("msg send") ] = msg_send_format;

	// msg send-from bob alice
	// msg send-from bob alice HelloThisIsATest // TODO, other call to OTUse, just pass the message
	// msg send-from bob alice "Hello This Is A Test" // TODO, need parser+editline support for quotes

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




} // namespace 
} // namespace 

