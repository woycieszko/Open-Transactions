/* See other files here for the LICENCE that applies here. */
/* See header file .hpp for info */

#include "useot.hpp"

#include <OTPaths.hpp>

#include "lib_common3.hpp"

namespace nOT {
namespace nUse {

INJECT_OT_COMMON_USING_NAMESPACE_COMMON_3; // <=== namespaces

cUseOT::cUseOT(const string &mDbgName)
: 
	mDbgName(mDbgName)
, mNymsMy_loaded(false)
, OTAPI_loaded(false)
, OTAPI_error(false)
, mDataFolder( OTPaths::AppDataFolder().Get() )
, mDefaultIDsFile( mDataFolder + "defaults.opt" )
{
	_dbg1("Creating cUseOT "<<DbgName());

	subjectGetIDFunc.insert(std::make_pair(subjectType::Account, AccountGetId);
	subjectGetIDFunc.insert(std::make_pair(subjectType::Asset, AssetGetId);
	subjectGetIDFunc.insert(std::make_pair(subjectType::Nym, NymGetId);
	subjectGetIDFunc.insert(std::make_pair(subjectType::Server, ServerGetId);
}


string cUseOT::DbgName() const noexcept {
	return "cUseOT-" + ToStr((void*)this) + "-" + mDbgName;
}

void cUseOT::CloseApi() {
	if (OTAPI_loaded) {
		_dbg1("Will cleanup OTAPI");
		OTAPI_Wrap::AppCleanup(); // UnInit OTAPI
		_dbg2("Will cleanup OTAPI - DONE");
	} else _dbg3("Will cleanup OTAPI ... was already not loaded");
}

cUseOT::~cUseOT() {
	CloseApi();
}

void cUseOT::LoadDefaults() {
	// TODO What if there is, for example no accounts?
	// TODO Check if defaults are correct.
	if ( !configManager.Load(mDefaultIDsFile, mDefaultIDs) ) {
		_dbg1("Cannot open" + mDefaultIDsFile + " file, setting IDs with ID 0 as default");
		mDefaultIDs["AccountID"] = OTAPI_Wrap::GetAccountWallet_ID(0);
		mDefaultIDs["PurseID"] = OTAPI_Wrap::GetAssetType_ID(0);
		mDefaultIDs["ServerID"] = OTAPI_Wrap::GetServer_ID(0);
		mDefaultIDs["UserID"] = OTAPI_Wrap::GetNym_ID(0);
	}
}

bool cUseOT::Init() {
	if (OTAPI_error) return false;
	if (OTAPI_loaded) return true;
	try {
		if (!OTAPI_Wrap::AppInit()) {// Init OTAPI
			_erro("Error while initializing wrapper");
			return false; // <--- RET
		}

		_info("Trying to load wallet now.");
		// if not pWrap it means that AppInit is not successed
		OTAPI_Exec *pWrap = OTAPI_Wrap::It(); // TODO check why OTAPI_Exec is needed
		if (!pWrap) {
			OTAPI_error = true;
			_erro("Error while init OTAPI (1)");
			return false;
		}

		if (OTAPI_Wrap::LoadWallet()) {
			_info("wallet was loaded.");
			OTAPI_loaded = true;
			LoadDefaults();
		}	else _erro("Error while loading wallet.");
	}
	catch(const std::exception &e) {
		_erro("Error while OTAPI init (2) - " << e.what());
		return false;
	}
	catch(...) {
		_erro("Error while OTAPI init thrown an UNKNOWN exception!");
		OTAPI_error = true;
		return false;
	}
	return OTAPI_loaded;
}

bool cUseOT::CheckIfExists(subjectType type, const string & subject) {
	if(!Init()) return false;

	ID subjectID = subjectGetIDFunc.at(type)(subject);

	if (!subjectID.empty()) {
		_dbg3("Account " + subject + " exists");
		return true;
	}
	_warn("Can't find this Account: " + subject);
	return false;
}

bool cUseOT::AccountCheckIfExists(const string & account) {
	if(!Init()) return false;

	ID accountID = AccountGetId(account);

	if (!accountID.empty()) {
		_dbg3("Account " + account + " exists");
		return true;
	}
	_warn("Can't find this Account: " + account);
	return false;
}

const vector<ID> cUseOT::AccountGetAllIds() {
	if(!Init())
	return vector<string> {};

	_dbg3("Retrieving accounts ID's");
	vector<string> accountsIDs;
	for(int i = 0 ; i < OTAPI_Wrap::GetAccountCount ();i++) {
		accountsIDs.push_back(OTAPI_Wrap::GetAccountWallet_ID (i));
	}
	return accountsIDs;
}

const int64_t cUseOT::AccountGetBalance(const string & accountName) {
	if(!Init())
		return 0; //FIXME

	int64_t balance = OTAPI_Wrap::GetAccountWallet_Balance	( AccountGetId(accountName) );
	return balance;
}

const string cUseOT::AccountGetDefault() {
	if(!Init())
		return "";
	return mDefaultIDs.at("AccountID");
}

const ID cUseOT::AccountGetId(const string & accountName) {
	if(!Init())
		return "";
	if ( nUtils::checkPrefix(accountName) )
		return accountName.substr(1);
	else {
		for(int i = 0 ; i < OTAPI_Wrap::GetAccountCount ();i++) {
			if(OTAPI_Wrap::GetAccountWallet_Name ( OTAPI_Wrap::GetAccountWallet_ID (i))==accountName)
			return OTAPI_Wrap::GetAccountWallet_ID (i);
		}
	}
	return "";
}

const string cUseOT::AccountGetName(const ID & accountID) {
	if(!Init())
		return "";
	return OTAPI_Wrap::GetAccountWallet_Name(accountID);
}

bool cUseOT::AccountRemove(const string & account, bool dryrun) { ///<
	_fact("account rm " << account);
	if(dryrun) return false;
	if(!Init()) return false;

	if(OTAPI_Wrap::Wallet_CanRemoveAccount (AccountGetId(account))) {
		_erro("Account cannot be deleted: doesn't have a zero balance?/outstanding receipts?");
		return false;
	}

	if( OTAPI_Wrap::deleteAssetAccount( mDefaultIDs.at("ServerID"), mDefaultIDs.at("UserID"), AccountGetId(account) ) ) { //FIXME should be
		_erro("Failure deleting account: " + account);
		return false;
	}
	_info("Account: " + account + " was successfully removed");
	return true;
}

bool cUseOT::AccountRefresh(const string & accountName, bool all, bool dryrun) {
	_fact("account refresh " << accountName << " all=" << all);
	if(dryrun) return false;
	if(!Init()) return false;

	OT_ME madeEasy;

	int32_t serverCount = OTAPI_Wrap::GetServerCount();

	if (all) {
		int32_t accountsRetrieved = 0;
		int32_t accountCount = OTAPI_Wrap::GetAccountCount();

		if (accountCount == 0){
			_warn("No accounts to retrieve");
			return true;
		}

		for (int32_t accountIndex = 0; accountIndex < accountCount; ++accountIndex) {
			ID accountID = OTAPI_Wrap::GetAccountWallet_ID(accountIndex);
			ID accountServerID = OTAPI_Wrap::GetAccountWallet_ServerID(accountID);
			ID accountNymID = OTAPI_Wrap::GetAccountWallet_NymID(accountID);
			if ( madeEasy.retrieve_account(accountServerID, accountNymID, accountID, true) ) { // forcing download
				_info("Account " + accountName + "(" + accountID +  ")" + " retrieval success from server " + ServerGetName(accountServerID) + "(" + accountServerID +  ")");
				++accountsRetrieved;
			}else
				_erro("Account " + accountName + "(" + accountID +  ")" + " retrieval failure from server " + ServerGetName(accountServerID) + "(" + accountServerID +  ")");
		}
		string count = to_string(accountsRetrieved) + "/" + to_string(accountCount);
		if (accountsRetrieved == accountCount) {
			_info("All accounts were successfully retrieved " << count);
			return true;
		} else if (accountsRetrieved == 0) {
			_erro("Accounts retrieval failure " << count);
			return false;
		} else {
			_erro("Some accounts cannot be retrieved " << count);
			return true;
		}
	}
	else {
		ID accountID = AccountGetId(accountName);
		ID accountServerID = OTAPI_Wrap::GetAccountWallet_ServerID(accountID);
		ID accountNymID = OTAPI_Wrap::GetAccountWallet_NymID(accountID);
		if ( madeEasy.retrieve_account(accountServerID, accountNymID, accountID, true) ) { // forcing download
			_info("Account " + accountName + "(" + accountID +  ")" + " retrieval success from server " + ServerGetName(accountServerID) + "(" + accountServerID +  ")");
			return true;
		}
		_warn("Account " + accountName + "(" + accountID +  ")" + " retrieval failure from server " + ServerGetName(accountServerID) + "(" + accountServerID +  ")");
		return false;
	}
	return false;
}

bool cUseOT::AccountRename(const string & account, const string & newAccountName, bool dryrun) {
	_fact("account mv from " << account << " to " << newAccountName);
	if(dryrun) return false;
	if(!Init()) return false;

	if( AccountSetName (AccountGetId(account), newAccountName) ) {
		_info("Account " << account << " renamed to " << newAccountName);
		return true;
	}
	_erro("Failed to rename account " << account << " to " << newAccountName);
	return false;
}

bool cUseOT::AccountSetName(const string & accountID, const string & newAccountName) { //TODO: passing to function: const string & nymName, const string & signerNymName,
	if(!Init()) return false;

	if ( !OTAPI_Wrap::SetAccountWallet_Name (accountID, mDefaultIDs.at("UserID"), newAccountName) ) {
		_erro("Failed trying to name new account: " << accountID);
		return false;
	}
	_info("Set account " << accountID << "name to " << newAccountName);
	return true;
}

bool cUseOT::AccountCreate(const string & assetName, const string & newAccountName, bool dryrun) {
	_fact("account new asset=" << assetName << "accountName=" << newAccountName);
	if(dryrun) return false;
	if(!Init()) return false;

	OT_ME madeEasy;
	string response;
	response = madeEasy.create_asset_acct(mDefaultIDs.at("ServerID"), mDefaultIDs.at("UserID"), AssetGetId(assetName));

	// -1 error, 0 failure, 1 success.
	if (1 != madeEasy.VerifyMessageSuccess(response))
	{
		_erro("Failed trying to create Account at Server.");
		return false;
	}

	// Get the ID of the new account.
	ID accountID = OTAPI_Wrap::Message_GetNewAcctID(response);
	if (!accountID.size()){
		_erro("Failed trying to get the new account's ID from the server response.");
		return false;
	}

	// Set the Name of the new account.
	if ( AccountSetName(accountID, newAccountName) ){
		cout << "Account " << newAccountName << "(" << accountID << ")" << " created successfully." << endl;
		return true;
	}
	return false;
}

const vector<string> cUseOT::AccountGetAllNames() {
	if(!Init())
	return vector<string> {};

	_dbg3("Retrieving all accounts names");
	vector<string> accounts;
	for(int i = 0 ; i < OTAPI_Wrap::GetAccountCount ();i++) {
		accounts.push_back(OTAPI_Wrap::GetAccountWallet_Name ( OTAPI_Wrap::GetAccountWallet_ID (i)));
	}
	return accounts;
}

bool cUseOT::AccountDisplayAllNames(bool dryrun) {
	_fact("account ls");
	if(dryrun) return false;
	if(!Init()) return false;

	_dbg3("Retrieving all accounts names");
	for(std::int32_t i = 0 ; i < OTAPI_Wrap::GetAccountCount();i++) {
		ID accountID = OTAPI_Wrap::GetAccountWallet_ID(i);
		nUtils::DisplayStringEndl(cout, AccountGetName( accountID ) + " - " + accountID );
	}
	return true;
}

bool cUseOT::AccountSetDefault(const string & account, bool dryrun) {
	_fact("account set-default " << account);
	if(dryrun) return false;
	if(!Init()) return false;

	mDefaultIDs.at("AccountID") = AccountGetId(account);
	return true;
}

bool cUseOT::AccountTransfer(const string & accountFrom, const string & accountTo, const int64_t & amount, const string & note, bool dryrun) {
	_fact("account transfer  from " << accountFrom << " to " << accountTo << " amount=" << amount << " note=" << note);
	if(dryrun) return false;
	if(!Init()) return false;

	OT_ME madeEasy;

	ID accountFromID = AccountGetId(accountFrom);
	ID accountToID = AccountGetId(accountTo);
	ID accountServerID = OTAPI_Wrap::GetAccountWallet_ServerID(accountFromID);
	ID accountNymID = OTAPI_Wrap::GetAccountWallet_NymID(accountFromID);

	string response = madeEasy.send_transfer(accountServerID, accountNymID, accountFromID, accountToID, amount, note);

	// -1 error, 0 failure, 1 success.
	if (1 != madeEasy.VerifyMessageSuccess(response)) {
		_erro("Failed to send transfer from " << accountFrom << " to " << accountTo);
		return false;
	}
	return true;
}

bool cUseOT::AssetCheckIfExists(const string & asset) {
	if(!Init()) return false;

	ID assetID = AssetGetId(asset);

	if (!assetID.empty()) {
		_dbg3("Asset " + asset + " exists");
		return true;
	}
	_warn("Can't find this asset: " + asset);
	return false;
}

const vector<string> cUseOT::AssetGetAllNames() {
	if(!Init())
	return vector<string> {};

	vector<string> assets;
	for(std::int32_t i = 0 ; i < OTAPI_Wrap::GetAssetTypeCount ();i++) {
		assets.push_back(OTAPI_Wrap::GetAssetType_Name ( OTAPI_Wrap::GetAssetType_ID (i)));
	}
	return assets;
}

const string cUseOT::AssetGetName(const ID & accountID) {
	if(!Init())
		return "";
	return OTAPI_Wrap::GetAccountWallet_Name(accountID);
}

bool cUseOT::AssetDisplayAllNames(bool dryrun) {
	_fact("asset ls");
	if(dryrun) return false;
	if(!Init()) return false;

	_dbg3("Retrieving all asset names");
	for(std::int32_t i = 0 ; i < OTAPI_Wrap::GetAssetTypeCount();i++) {
		ID assetID = OTAPI_Wrap::GetAssetType_ID(i);
		nUtils::DisplayStringEndl(cout, AssetGetName( assetID ) + " - " + assetID );
	}
	return true;
}

const string cUseOT::AssetGetId(const string & assetName) {
	if(!Init())
		return "";
	if ( nUtils::checkPrefix(assetName) )
		return assetName.substr(1);
	else {
		for(std::int32_t i = 0 ; i < OTAPI_Wrap::GetAssetTypeCount ();i++) {
			if(OTAPI_Wrap::GetAssetType_Name ( OTAPI_Wrap::GetAssetType_ID (i))==assetName)
				return OTAPI_Wrap::GetAssetType_ID (i);
		}
	}
	return "";
}

const string cUseOT::AssetGetContract(const string & asset){
	if(!Init())
		return "";
	string strContract = OTAPI_Wrap::GetAssetType_Contract( AssetGetId(asset) );
	return strContract;
}

const string cUseOT::AssetGetDefault(){
	return mDefaultIDs.at("PurseID");
}

bool cUseOT::AssetIssue(const string & serverID, const string & nymID, bool dryrun) { // Issue new asset type
	_fact("asset ls");
	if(dryrun) return false;
	if(!Init()) return false;

	string signedContract;
	_dbg3("Message is empty, starting text editor");
	nUtils::cEnvUtils envUtils;
	signedContract = envUtils.Compose();

	OT_ME madeEasy;
	//std::string OT_ME::issue_asset_type(const std::string  & SERVER_ID, const std::string  & NYM_ID, const std::string  & THE_CONTRACT)
	string strResponse = madeEasy.issue_asset_type(serverID, nymID, signedContract);

	// -1 error, 0 failure, 1 success.
	if (1 != madeEasy.VerifyMessageSuccess(strResponse))
	{
		_erro("Failed trying to issue asset at Server.");
		return false;
	}
	return true;
}

bool cUseOT::AssetNew(const string & nym, bool dryrun) {
	_fact("asset new for nym=" << nym);
	if(dryrun) return false;
	if(!Init()) return false;
	string xmlContents;
	nUtils::cEnvUtils envUtils;
	xmlContents = envUtils.Compose();

	nUtils::DisplayStringEndl(cout, OTAPI_Wrap::CreateAssetContract(NymGetId(nym), xmlContents) ); //TODO save contract to file
	return true;
}

bool cUseOT::AssetRemove(const string & asset, bool dryrun) {
	_fact("asset rm " << asset);
	if(dryrun) return false;
	if(!Init()) return false;

	string assetID = AssetGetId(asset);
	if ( OTAPI_Wrap::Wallet_CanRemoveAssetType(assetID) ) {
		if ( OTAPI_Wrap::Wallet_RemoveAssetType(assetID) ) {
			_info("Asset was deleted successfully");
			return true;
		}
	}
	_warn("Asset cannot be removed");
	return false;
}

void cUseOT::AssetSetDefault(const std::string & assetName){
	if(!Init())
		return ;
	mDefaultIDs.at("PurseID") = AssetGetId(assetName);
}

const string cUseOT::ContractSign(const std::string & nymID, const std::string & contract){ // FIXME can't sign contract with this (assetNew() functionality)
	if(!Init())
		return "";
	return OTAPI_Wrap::AddSignature(nymID, contract);
}

const vector<string> cUseOT::MsgGetAll() { ///< Get all messages from all Nyms.
	if(!Init())
	return vector<string> {};

	for(int i = 0 ; i < OTAPI_Wrap::GetNymCount ();i++) {
		MsgDisplayForNym( NymGetName( OTAPI_Wrap::GetNym_ID(i) ), false );
	}
	return vector<string> {};
}

bool cUseOT::MsgDisplayForNym(const string & nymName, bool dryrun) { ///< Get all messages from Nym.
	_fact("msg ls " << nymName);
		if (dryrun) return false;

	if(!Init())
		return false;
	string nymID = NymGetId(nymName);
	cout << "===" << nymName << "(" << nymID << ")"  << "===" << endl;
	cout << "INBOX" << endl;
	cout << "id\tfrom\t\tcontent:" << endl;
	for(int i = 0 ; i < OTAPI_Wrap::GetNym_MailCount (nymID);i++) {
		cout << i << "\t" << OTAPI_Wrap::GetNym_Name(OTAPI_Wrap::GetNym_MailSenderIDByIndex(nymID, i))  << "\t" << OTAPI_Wrap::GetNym_MailContentsByIndex (nymID,i) << endl;
	}
	cout << "OUTBOX" << endl;
	cout << "id\tto\t\tcontent:" << endl;
	for(int i = 0 ; i < OTAPI_Wrap::GetNym_OutmailCount (nymID);i++) {
		cout << i << "\t" << OTAPI_Wrap::GetNym_Name(OTAPI_Wrap::GetNym_OutmailRecipientIDByIndex(nymID, i)) << "\t" << OTAPI_Wrap::GetNym_OutmailContentsByIndex (nymID,i) << endl;
	}

	return true;
}

bool cUseOT::MsgSend(const string & nymSender, vector<string> nymRecipient, const string & msg, const string & subject, int prio, bool dryrun) {
	_fact("MsgSend " << nymSender << " to " << DbgVector(nymRecipient) << " msg=" << msg << " subj="<<subject<<" prio="<<prio);
	if(dryrun) return false;
	if(!Init()) return false;

	string outMsg;

	if ( msg.empty() ) {
		_dbg3("Message is empty, starting text editor");
		nUtils::cEnvUtils envUtils;
		outMsg = envUtils.Compose();
	}
	else
		outMsg = msg;

	if ( outMsg.empty() ) {
		_warn("Can't send the message: message is empty");
		return false;
	}

	OT_ME madeEasy;

	ID senderID = NymGetId(nymSender);
	vector<ID> recipientID;
	for (auto varName : nymRecipient)
		recipientID.push_back( NymGetId(varName) );

	for (auto varID : recipientID) {
		_dbg1("Sending message from " + senderID + " to " + varID );

		string strResponse = madeEasy.send_user_msg ( mDefaultIDs.at("ServerID"), senderID, varID, outMsg);

		// -1 error, 0 failure, 1 success.
		if (1 != madeEasy.VerifyMessageSuccess(strResponse))
		{
			_erro("Failed trying to send the message");
			return false;
		}
		_dbg3("Message from " + senderID + " to " + varID + " was sent successfully.");
	}
	_info("All messages were sent successfully.");
	return true;
}

//bool cUseOT::MsgSend(const string & nymSender, const string & nymRecipient, const string & msg) { ///< Send message from Nym1 to Nym2
//	_note("MsgSend " << nymSender << " to " << nymRecipient << " msg=" << msg);
//
//	if(!Init())
//		return false;
//
//	string subject = "";
//	int prio = 0;
//	bool dryrun = false;
//	return MsgSend(nymSender, vector<string> {nymRecipient}, msg, subject, prio, dryrun);
//}

bool cUseOT::MsgInCheckIndex(const string & nymName, const int32_t & nIndex) {
	if(!Init())
			return false;
	if ( nIndex >= 0 && nIndex < OTAPI_Wrap::GetNym_MailCount(NymGetId(nymName)) ) {
		return true;
	}
	return false;
}

bool cUseOT::MsgOutCheckIndex(const string & nymName, const int32_t & nIndex) {
	if(!Init())
			return false;
	if ( nIndex >= 0 && nIndex < OTAPI_Wrap::GetNym_OutmailCount(NymGetId(nymName)) ) {
		return true;
	}
	return false;
}

bool cUseOT::MsgInRemoveByIndex(const string & nymName, const int32_t & index, bool dryrun) {
	_fact("msg rm " << nymName << " index=" << index);
	if (dryrun) return false;
	if(!Init()) return false;
	if(OTAPI_Wrap::Nym_RemoveMailByIndex (NymGetId(nymName), index)){
		_info("Message " << index << " removed successfully from " << nymName << " inbox");
	return true;
	}
	return false;
}

bool cUseOT::MsgOutRemoveByIndex(const string & nymName, const int32_t & index, bool dryrun) {
	_fact("msg rm-out " << nymName << " index=" << index);
	if (dryrun) return false;
	if(!Init()) return false;
	if( OTAPI_Wrap::Nym_RemoveOutmailByIndex(NymGetId(nymName), index) ) {
		_info("Message " << index << " removed successfully from " << nymName << " outbox");
		return true;
	}
	return false;
}

bool cUseOT::NymCheck(const string & nymName, bool dryrun) { // wip
	_fact("nym check " << nymName);
	if (dryrun) return false;
	if(!Init()) return false;

	ID nymID = NymGetId(nymName);
	OT_ME madeEasy;
	string strResponse = madeEasy.check_user( mDefaultIDs.at("ServerID"), mDefaultIDs.at("UserID"), nymID );
	// -1 error, 0 failure, 1 success.
	if (1 != madeEasy.VerifyMessageSuccess(strResponse)) {
		_erro("Failed trying to download public key for nym: " << nymName << "(" << nymID << ")" );
		return false;
	}
	_info("Successfully downloaded user public key for nym: " << nymName << "(" << nymID << ")" );
	return true;
}

bool cUseOT::NymCreate(const string & nymName, bool dryrun) {
	_fact("nym create " << nymName);
	if (dryrun) return false;
	if(!Init()) return false;

	OT_ME madeEasy;
	int32_t nKeybits = 1024;
	string NYM_ID_SOURCE = ""; //TODO: check
	string ALT_LOCATION = "";
	string nymID = madeEasy.create_pseudonym(nKeybits, NYM_ID_SOURCE, ALT_LOCATION);

	if (nymID.empty()) {
		_erro("Failed trying to create new Nym: " << nymName);
		return false;
	}
	// Set the Name of the new Nym.

	if ( !OTAPI_Wrap::SetNym_Name(nymID, nymID, nymName) ) { //Signer Nym? When testing, there is only one nym, so you just pass it twice. But in real production, a user will have a default signing nym, the same way that he might have a default signing key in PGP, and that must be passed in whenever he changes the name on any of the other nyms in his wallet. (In order to properly sign and save the change.)
		_erro("Failed trying to name new Nym: " << nymID);
		return false;
	}
	_info("Nym " << nymName << "(" << nymID << ")" << " created successfully.");
	//	TODO add nym to the cache
	return true;
}

bool cUseOT::NymCheckIfExists(const string & nym) {
	if(!Init()) return false;

	ID nymID = NymGetId(nym);

	if (!nymID.empty()) {
		_dbg3("Nym " + nym + " exists");
		return true;
	}
	_warn("Can't find this Nym: " + nym);
	return false;
}

void cUseOT::NymGetAll() {
	if(!Init())
		return;

	if (mNyms.size() != OTAPI_Wrap::GetNymCount()) { //TODO optimize?
		mNyms.clear();

		for(int i = 0 ; i < OTAPI_Wrap::GetNymCount();i++) {
			string nym_ID = OTAPI_Wrap::GetNym_ID (i);
			string nym_Name = OTAPI_Wrap::GetNym_Name (nym_ID);

			mNyms.insert( std::make_pair(nym_ID, nym_Name) );
		}
	}
}

const vector<string> cUseOT::NymGetAllIDs() {
	if(!Init())
		return vector<string> {};
	NymGetAllNames();
	vector<string> IDs;
	for (auto val : mNyms) {
		IDs.push_back(val.first);
	}
	return IDs;
}

const vector<string> cUseOT::NymGetAllNames() {
	if(!Init())
		return vector<string> {};
	NymGetAll();
	vector<string> names;
	for (auto val : mNyms) {
		names.push_back(val.second);
	}
	return names;
}

bool cUseOT::NymDisplayAllNames(bool dryrun) {
	_fact("nym ls ");
	if(dryrun) return false;
	if(!Init()) return false;

	NymGetAll();
	nUtils::DisplayMap(cout, mNyms);// display Nyms cache

	return true;
}

const string cUseOT::NymGetDefault() {
	if(!Init())
		return "";
	return mDefaultIDs.at("UserID");
}

const string cUseOT::NymGetId(const string & nymName) { // Gets nym aliases and IDs begins with '^'
	if(!Init())
		return "";
	if ( nUtils::checkPrefix(nymName) ) // nym ID
		return nymName.substr(1);
	else { // nym Name
		for(int i = 0 ; i < OTAPI_Wrap::GetNymCount ();i++) {
			string nymID = OTAPI_Wrap::GetNym_ID (i);
			string nymName_ = OTAPI_Wrap::GetNym_Name (nymID);
			if (nymName_ == nymName)
				return nymID;
		}
	}
	return "";
}

bool cUseOT::NymDisplayInfo(const string & nymName, bool dryrun) {
	_fact("nym info " << nymName);
	if(dryrun) return false;
	if(!Init()) return false;

	cout << OTAPI_Wrap::GetNym_Stats( NymGetId(nymName) );
	return true;
}

const string cUseOT::NymGetName(const string & nymID) {
	if(!Init())
		return "";
	return OTAPI_Wrap::GetNym_Name(nymID);
}

bool cUseOT::NymRefresh(const string & nymName, bool all, bool dryrun) { //TODO arguments for server, all servers
	_fact("nym refresh " << nymName << " all?=" << all);
	if(dryrun) return false;
	if(!Init()) return false;

	OT_ME madeEasy;
	int32_t serverCount = OTAPI_Wrap::GetServerCount();
	if (all) {
		int32_t nymsRetrieved = 0;
		int32_t nymCount = OTAPI_Wrap::GetNymCount();
		if (nymCount == 0){
			_warn("No Nyms to retrieve");
			return true;
		}

		for (int32_t serverIndex = 0; serverIndex < serverCount; ++serverIndex) { // FIXME Working for all available servers!
			for (int32_t nymIndex = 0; nymIndex < nymCount; ++nymIndex) {
				ID nymID = OTAPI_Wrap::GetNym_ID(nymIndex);
				ID serverID = OTAPI_Wrap::GetServer_ID(serverIndex);
				if (OTAPI_Wrap::IsNym_RegisteredAtServer(nymID, serverID)) {
					if ( madeEasy.retrieve_nym(serverID, nymID, true) ){ // forcing download
						_info("Nym " + NymGetName(nymID) + "(" + nymID +  ")" + " retrieval success from server " + ServerGetName(serverID) + "(" + serverID +  ")");
						++nymsRetrieved;
					} else
					_erro("Nym " + NymGetName(nymID) + "(" + nymID +  ")" + " retrieval failure from server " + ServerGetName(serverID) + "(" + serverID +  ")");
				}
			}
		}
		string count = to_string(nymsRetrieved) + "/" + to_string(nymCount);
		if (nymsRetrieved == nymCount) {
			_info("All Nyms were successfully retrieved " << count);
			return true;
		} else if (nymsRetrieved == 0) {
			_erro("Nyms retrieval failure " << count);
			return false;
		} else {
			_erro("Some Nyms cannot be retrieved (not registered?) " << count); //TODO check if nym is regstered on server
			return true;
		}
	}
	else {
		ID nymID = NymGetId(nymName);
		for (int32_t serverIndex = 0; serverIndex < serverCount; ++serverIndex) { // Working for all available servers!
			ID serverID = OTAPI_Wrap::GetServer_ID(serverIndex);
			if (OTAPI_Wrap::IsNym_RegisteredAtServer(nymID, serverID)) {
				if ( madeEasy.retrieve_nym(serverID,nymID, true) ) { // forcing download
					_info("Nym " + nymName + "(" + nymID +  ")" + " retrieval success from server " + ServerGetName(serverID) + "(" + serverID +  ")");
					return true;
				}
				_warn("Nym " + nymName + "(" + nymID +  ")" + " retrieval failure from server " + ServerGetName(serverID) + "(" + serverID +  ")");
				return false;
			}
		}
	}
	return false;
}

bool cUseOT::NymRegister(const string & nymName, const string & serverName, bool dryrun) {
	_fact("nym register " << nymName << " on server " << serverName);
	if(dryrun) return false;
	if(!Init()) return false;

	OT_ME madeEasy;

	ID nymID = NymGetId(nymName);
	ID serverID = ServerGetId(serverName);

	if (!OTAPI_Wrap::IsNym_RegisteredAtServer(nymID, serverID)) {
		string response = madeEasy.register_nym(serverID, nymID);
		nOT::nUtils::DisplayStringEndl(cout, response);
		_info("Nym " << nymName << "(" << nymID << ")" << " was registered successfully on server");
		return true;
	}
	_info("Nym " << nymName << "(" << nymID << ")" << " was already registered" << endl);
	return true;
}

bool cUseOT::NymRemove(const string & nymName, bool dryrun) {
	_fact("nym rm " << nymName);
	if(dryrun) return false;
	if(!Init()) return false;

	string nymID = NymGetId(nymName);
	if ( OTAPI_Wrap::Wallet_CanRemoveNym(nymID) ) {
		if ( OTAPI_Wrap::Wallet_RemoveNym(nymID) ) {
			_info("Nym " << nymName  <<  "(" << nymID << ")" << " was deleted successfully");
			return true;
		}
	}
	_warn("Nym " << nymName  <<  "(" << nymID << ")" << " cannot be removed");
	return false;
}

bool cUseOT::NymSetName(const string & nymID, const string & newNymName) { //TODO: passing to function: const string & nymName, const string & signerNymName,
	if(!Init()) return false;

	if ( !OTAPI_Wrap::SetNym_Name(nymID, nymID, newNymName) ) {
		_erro("Failed trying to set name " << newNymName << " to nym " << nymID);
		return false;
	}
	_info("Set Nym " << nymID << " name to " << newNymName);
	return true;
}

bool cUseOT::NymRename(const string & oldNymName, const string & newNymName, bool dryrun) {
	_fact("nym rename from " << oldNymName << " to " << newNymName);
	if(dryrun) return false;
	if(!Init()) return false;

	if( NymSetName(NymGetId(oldNymName), newNymName) ) {
		_info("Nym " << oldNymName << " renamed to " << newNymName);
		return true;
	}
	_erro("Failed to rename Nym " << oldNymName << " to " << newNymName);
	return false;
}

bool cUseOT::NymSetDefault(const string & nymName, bool dryrun) {
	_fact("nym set-default " << nymName);
	if(dryrun) return false;
	if(!Init()) return false;

	mDefaultIDs.at("UserID") = NymGetId(nymName);
	return true;
}

bool cUseOT::ServerAdd(bool dryrun) {
	_fact("server ls");
	if(dryrun) return false;
	if(!Init()) return false;

	string contract;
	nUtils::cEnvUtils envUtils;
	contract = envUtils.Compose();

	if( OTAPI_Wrap::AddServerContract(contract) ) {
		_info("Server added");
		return true;
	}
	_erro("Failure to add server");
	return false;
}

bool cUseOT::ServerCreate(const string & nymName, bool dryrun) {
	_fact("server ls");
	if(dryrun) return false;
	if(!Init()) return false;

	string xmlContents;
	nUtils::cEnvUtils envUtils;
	xmlContents = envUtils.Compose();

	ID nymID = NymGetId(nymName);
	string contract = OTAPI_Wrap::CreateServerContract(nymID, xmlContents);

	if( !contract.empty() ) {
		_info( "Contract created for Nym: " << nymName << "(" << nymID << ")" );
		nUtils::DisplayStringEndl(cout, contract);
		return true;
	}
	_erro( "Failure to create contract for nym: " << nymName << "(" << nymID << ")" );
	return false;
}

void cUseOT::ServerCheck() {
	if(!Init())
			return ;

	if( !OTAPI_Wrap::checkServerID( mDefaultIDs.at("ServerID"), mDefaultIDs.at("UserID") ) ) {
		_erro( "No response from server: " + mDefaultIDs.at("ServerID") );
	}
	_info("Server " + mDefaultIDs.at("ServerID") + " is OK");
}

bool cUseOT::ServerCheckIfExists(const string & server) {
	if(!Init()) return false;

	ID serverID = ServerGetId(server);

	if (!serverID.empty()) {
		_dbg3("Server " + server + " exists");
		return true;
	}
	_warn("Can't find this server: " + server);
	return false;
}


const string cUseOT::ServerGetDefault() {
	if(!Init())
		return "";
	return mDefaultIDs.at("ServerID");
}

const string cUseOT::ServerGetId(const string & serverName) { ///< Gets nym aliases and IDs begins with '%'
	if(!Init())
		return "";

	if ( nUtils::checkPrefix(serverName) )
		return serverName.substr(1);
	else {
		for(int i = 0 ; i < OTAPI_Wrap::GetServerCount(); i++) {
			string serverID = OTAPI_Wrap::GetServer_ID(i);
			string serverName_ = OTAPI_Wrap::GetServer_Name(serverID);
			if (serverName_ == serverName)
				return serverID;
		}
	}
	return "";
}

const string cUseOT::ServerGetName(const string & serverID){
	if(!Init())
		return "";
	return OTAPI_Wrap::GetServer_Name(serverID);
}

bool cUseOT::ServerRemove(const string & serverName, bool dryrun) {
	_fact("server rm " << serverName);
	if(dryrun) return false;
	if(!Init()) return false;
	string serverID = ServerGetId(serverName);
	if ( OTAPI_Wrap::Wallet_CanRemoveServer(serverID) ) {
		if ( OTAPI_Wrap::Wallet_RemoveServer(serverID) ) {
			_info("Server " << serverName << " was deleted successfully");
			return true;
		}
		_warn("Failed to remove server " << serverName);
		return false;
	}
	_warn("Server " << serverName << " cannot be removed");
	return false;
}

bool cUseOT::ServerSetDefault(const string & serverName, bool dryrun) {
	_fact("server set-default " << serverName);
	if(dryrun) return false;
	if(!Init()) return false;

	mDefaultIDs.at("ServerID") = ServerGetId(serverName);
	return true;
}

const vector<string> cUseOT::ServerGetAllNames() { ///< Gets all servers name
	if(!Init())
	return vector<string> {};

	vector<string> servers;
	for(int i = 0 ; i < OTAPI_Wrap::GetServerCount ();i++) {
		string servID = OTAPI_Wrap::GetServer_ID(i);
		servers.push_back(servID);
	}
	return servers;
}

bool cUseOT::ServerDisplayAllNames(bool dryrun) {
	_fact("server ls");
	if(dryrun) return false;
	if(!Init()) return false;


	for(std::int32_t i = 0 ; i < OTAPI_Wrap::GetServerCount();i++) {
		ID serverID = OTAPI_Wrap::GetServer_ID(i);
		nUtils::DisplayStringEndl(cout, ServerGetName( serverID ) + " - " + serverID );
	}
	return true;
}

bool cUseOT::TextEncode(const string & plainText, bool dryrun) {
	_fact("text encode");
	if(dryrun) return false;
	if(!Init()) return false;

	string plainTextIn;
	if ( plainText.empty() ) {
		nUtils::cEnvUtils envUtils;
		plainTextIn = envUtils.Compose();
	}
	else
		plainTextIn = plainText;

	bool bLineBreaks = true; // FIXME? OTAPI_Wrap - bLineBreaks should usually be set to true
	string encodedText;
	encodedText = OTAPI_Wrap::Encode (plainTextIn, bLineBreaks);
	nUtils::DisplayStringEndl(cout, encodedText);
	return true;
}

bool cUseOT::TextEncrypt(const string & recipientNymName, const string & plainText, bool dryrun) {
	_fact("text encrypt to " << recipientNymName);
	if(dryrun) return false;
	if(!Init()) return false;

	string plainTextIn;
	if ( plainText.empty() ) {
		nUtils::cEnvUtils envUtils;
		plainTextIn = envUtils.Compose();
	}
	else
		plainTextIn = plainText;

	string encryptedText;
	encryptedText = OTAPI_Wrap::Encrypt(NymGetId(recipientNymName), plainTextIn);
	nUtils::DisplayStringEndl(cout, encryptedText);
	return true;
}

bool cUseOT::TextDecode(const string & encodedText, bool dryrun) {
	_fact("text decode");
	if(dryrun) return false;
	if(!Init()) return false;

	string encodedTextIn;
	if ( encodedText.empty() ) {
		nUtils::cEnvUtils envUtils;
		encodedTextIn = envUtils.Compose();
	}
	else
		encodedTextIn = encodedText;

	bool bLineBreaks = true; // FIXME? OTAPI_Wrap - bLineBreaks should usually be set to true
	string plainText;
	plainText = OTAPI_Wrap::Decode (encodedTextIn, bLineBreaks);
	nUtils::DisplayStringEndl(cout, plainText);
	return true;
}

bool cUseOT::TextDecrypt(const string & recipientNymName, const string & encryptedText, bool dryrun) {
	_fact("text decrypt for " << recipientNymName );
	if(dryrun) return false;
	if(!Init()) return false;

	string encryptedTextIn;
		if ( encryptedText.empty() ) {
			nUtils::cEnvUtils envUtils;
			encryptedTextIn = envUtils.Compose();
		}
		else
			encryptedTextIn = encryptedText;

	string plainText;
	plainText = OTAPI_Wrap::Decrypt(NymGetId(recipientNymName), encryptedTextIn);
	nUtils::DisplayStringEndl(cout, plainText);
	return true;
}

} // nUse
}; // namespace OT


