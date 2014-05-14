/* See other files here for the LICENCE that applies here. */
/* See header file .hpp for info */

#include "useot.hpp"

#include "lib_common3.hpp"

namespace nOT {
namespace nUse {

INJECT_OT_COMMON_USING_NAMESPACE_COMMON_3; // <=== namespaces

cUseOT::cUseOT()
: mNymsMy_loaded(false)
, OTAPI_loaded(false)
, OTAPI_error(false)
, mServerID("r1fUoHwJOWCuK3WBAAySjmKYqsG6G2TYIxdqY6YNuuG")// TODO read all default ID's
, mUserID("DYEB6U7dcpbwdGrftPnslNKz76BDuBTFAjiAgKaiY2n")
, mAccountID("yQGh0vgm9YiqYOh6bfLDxyAA7Nnh2NmturCQmOt4LTo")
, mPurseID("JY7kEFnm2c50CGNphdpgnlVo2hOZuLrBua5cFqWWR9Q")
{}

cUseOT::~cUseOT() {
	if (OTAPI_loaded) OTAPI_Wrap::AppCleanup(); // UnInit OTAPI
}

bool cUseOT::Init() {
	if (OTAPI_error) return false;
	if (OTAPI_loaded) return true;
	try {
		if (!OTAPI_Wrap::AppInit()) {// Init OTAPI
			_erro("Error while init OTAPI thrown an UNKNOWN exception!");
			return false; // <--- RET
		}

		_info("Trying to load wallet: ");
		//if not pWrap it means that AppInit is not successed
		OTAPI_Exec *pWrap = OTAPI_Wrap::It(); // TODO check why OTAPI_Exec is needed
		if (!pWrap) {
			OTAPI_error = true;
			_erro("Error while init OTAPI (1)");
			return false;
		}

		if(OTAPI_Wrap::LoadWallet()) {
			_info("wallet was loaded.");
			OTAPI_loaded = true;
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

bool cUseOT::accountCheckIfExists(const string & accountName) {
	vector<string> v = accountsGet();
	if (std::find(v.begin(), v.end(), accountName) != v.end())
		return true;
	return false;
}

const string cUseOT::accountGetId(const string & accountName) {
	if(!Init())
	return "";

	for(int i = 0 ; i < OTAPI_Wrap::GetAccountCount ();i++) {
		if(OTAPI_Wrap::GetAccountWallet_Name ( OTAPI_Wrap::GetAccountWallet_ID (i))==accountName)
		return OTAPI_Wrap::GetAccountWallet_ID (i);
	}
	return "";
}
const string cUseOT::accountGetDefault() {
	if(!Init())
		return "";
	return mAccountID;
}

string cUseOT::accountDelete(const string & accountName) { ///<
	if(!Init())
	return "";

	if(!OTAPI_Wrap::Wallet_CanRemoveAccount (accountGetId(accountName))) {
		// inBox and OutBox must be get from server because without it account not work properly
		// example if you can't delete account without inbox and outbox
		int32_t  inBoxInt = OTAPI_Wrap::getInbox 	(mServerID,mUserID,accountGetId(accountName));
		int32_t outBoxInt = OTAPI_Wrap::getOutbox 	(mServerID,mUserID,accountGetId(accountName));
	}

	if(OTAPI_Wrap::deleteAssetAccount(mServerID, mUserID, accountGetId(accountName))==-1)
		return "Error while deleting account";
	else
		return "";
}

const vector<string> cUseOT::accountGetIds() {
	if(!Init())
	return vector<string> {};

	_dbg3("Retrieving accounts ID's");
	vector<string> accountsIDs;
	for(int i = 0 ; i < OTAPI_Wrap::GetAccountCount ();i++) {
		accountsIDs.push_back(OTAPI_Wrap::GetAccountWallet_ID (i));
	}
	return accountsIDs;
}

const string cUseOT::accountRename(const string & oldAccountName, const string & newAccountName) {

		accountSetName (accountGetId(oldAccountName), newAccountName);
	return "";
}

const string cUseOT::accountSetName(const string & accountID, const string & NewAccountName) { //TODO: passing to function: const string & nymName, const string & signerNymName,
	if(!Init())
	return "";

		OTAPI_Wrap::SetAccountWallet_Name (accountID, mUserID, NewAccountName);
	return "";
}

void cUseOT::accountCreate(const string & assetName, const string & newAccountName) {
	if(!Init())
	return ;

	OT_ME madeEasy;
	string strResponse;
	strResponse = madeEasy.create_asset_acct(mServerID, mUserID, assetGetId(assetName));

	// -1 error, 0 failure, 1 success.
	if (1 != madeEasy.VerifyMessageSuccess(strResponse))
	{
		_erro("Failed trying to create Account at Server.");
		return;
	}

	// Get the ID of the new account.
	string strID = OTAPI_Wrap::Message_GetNewAcctID(strResponse);
	if (!strID.size()){
		_erro("Failed trying to get the new account's ID from the server response.");
		return;
	}

	// Set the Name of the new account.
	accountSetName(strID,newAccountName);

	cout << "Account " << newAccountName << "(" << strID << ")" << " created successfully." << endl;
}
void cUseOT::accountSetDefault(const string & accountName) {
	if(!Init())
		return ;
	mAccountID = accountGetId(accountName);
}

const vector<string> cUseOT::accountsGet() {
	if(!Init())
	return vector<string> {};

	_dbg3("Retrieving accounts names");
	vector<string> accounts;
	for(int i = 0 ; i < OTAPI_Wrap::GetAccountCount ();i++) {
		accounts.push_back(OTAPI_Wrap::GetAccountWallet_Name ( OTAPI_Wrap::GetAccountWallet_ID (i)));
	}
	return accounts;
}

bool cUseOT::assetCheckIfExists(const string & assetName) {
	vector<string> v = assetsGetNames();
	if (std::find(v.begin(), v.end(), assetName) != v.end())
		return true;
	return false;
}

const string cUseOT::assetGetId(const string & assetName) {
	if(!Init())
	return "";

	for(int i = 0 ; i < OTAPI_Wrap::GetAssetTypeCount ();i++) {
		if(OTAPI_Wrap::GetAssetType_Name ( OTAPI_Wrap::GetAssetType_ID (i))==assetName)
			return OTAPI_Wrap::GetAssetType_ID (i);
	}
	return "";
}

const vector<string> cUseOT::assetsGetNames() {
	if(!Init())
	return vector<string> {};

	vector<string> assets;
	for(int i = 0 ; i < OTAPI_Wrap::GetAssetTypeCount ();i++) {
		assets.push_back(OTAPI_Wrap::GetAssetType_Name ( OTAPI_Wrap::GetAssetType_ID (i)));
	}
	return assets;
}

const vector<string> cUseOT::msgGetAll() { ///< Get all messages from all Nyms.
	if(!Init())
	return vector<string> {};

	for(int i = 0 ; i < OTAPI_Wrap::GetNymCount ();i++) {
		msgGetForNym( "^" + OTAPI_Wrap::GetNym_ID(i) );
	}
	return vector<string> {};
}

const vector<string> cUseOT::msgGetForNym(const string & nymName) { ///< Get all messages from Nym.
	if(!Init())
		return vector<string> {};
	string nymID = nymGetId(nymName);
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

	return vector<string> {};
}

void cUseOT::msgSend(const string & nymSender, const string & nymRecipient, const string & msg) { ///< Send message from Nym1 to Nym2
	if(!Init())
		return;

	OT_ME madeEasy;
	string sender = nymGetId(nymSender);
	string recipient = nymGetId(nymRecipient);

	_dbg1(sender);
	_dbg1(recipient);

	string strResponse = madeEasy.send_user_msg ( mServerID, sender, recipient, msg);

	// -1 error, 0 failure, 1 success.
	if (1 != madeEasy.VerifyMessageSuccess(strResponse))
	{
		_erro("Failed trying to send the message");
		return;
	}

	_info("Message was sent successfully.");
}

void cUseOT::msgRemoveByIndex(const string & nymName, const int32_t & nIndex) {
	if(OTAPI_Wrap::Nym_RemoveMailByIndex (nymGetId(nymName), nIndex)){
		_info("Message removed successfully");
	}
}

void cUseOT::nymCheck(const string & hisNymID) { // wip
	if(!Init())
		return;

	OT_ME madeEasy;
	string strResponse = madeEasy.check_user( mServerID, mUserID, hisNymID );
	// -1 error, 0 failure, 1 success.
	if (1 != madeEasy.VerifyMessageSuccess(strResponse))
	{
		_erro("Failed trying to download user public key.");
		return;
	}
	_info("Successfully downloaded user public key.");
}

bool cUseOT::nymCheckByName(const string & nymName) {
	vector<string> v = nymsGetMy();
	if (std::find(v.begin(), v.end(), nymName) != v.end())
		return true;
	return false;
}

void cUseOT::nymCreate(const string & nymName) {
	if(!Init())
	return ;

	OT_ME madeEasy;
	int32_t nKeybits = 1024;
	string NYM_ID_SOURCE = ""; //TODO: check
	string ALT_LOCATION = "";
	string strID = madeEasy.create_pseudonym(nKeybits, NYM_ID_SOURCE, ALT_LOCATION);

	if (strID.empty())
	{
		_erro("Failed trying to create new Nym.");
		return;
	}

	// Set the Name of the new Nym.
	OTAPI_Wrap::SetNym_Name(strID, strID, nymName);

	_info("Nym " << nymName << "(" << strID << ")" << " created successfully.");
}

const string cUseOT::nymGetDefault() {
	if(!Init())
		return "";
	return mUserID;
}

const string cUseOT::nymGetId(const string & nymName) { // Gets nym aliases and IDs begins with '^'
	if(!Init())
	return "";
	const char prefix = '^';
	if (nymName.at(0) == prefix) { // nym ID
		return nymName.substr(1);
	}
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

const string cUseOT::nymGetInfo(const string & nymName) {
	if(!Init())
		return "";

	if (nymCheckByName(nymName)){
		return OTAPI_Wrap::GetNym_Stats( nymGetId(nymName) );
	}
	else {
		_erro("Nym not found");
	}

	return "";
}

void cUseOT::nymRefresh() {
	if(!Init())
		return;

	// Retrieve Nyms based on Moneychanger

	OT_ME madeEasy;

	int32_t nymCount = OTAPI_Wrap::GetNymCount();

	int32_t serverCount = OTAPI_Wrap::GetServerCount();

	for (int32_t serverIndex = 0; serverIndex < serverCount; ++serverIndex)
	{
		string serverId = OTAPI_Wrap::GetServer_ID(serverIndex);

		for (int32_t nymIndex = 0; nymIndex < nymCount; ++nymIndex)
		{
			string nymId = OTAPI_Wrap::GetNym_ID(nymIndex);

			bool bRetrievalAttempted = false;
			bool bRetrievalSucceeded = false;

			if (OTAPI_Wrap::IsNym_RegisteredAtServer(nymId, serverId))
			{

				bRetrievalAttempted = true;
				bRetrievalSucceeded = madeEasy.retrieve_nym(serverId, nymId, true);
			}
		}
	}

}

void cUseOT::nymRegister(const string & nymName) {
	if(!Init())
	return ;

	OT_ME madeEasy;

	_warn("Checking for default server only");

	string nymID = nymGetId(nymName);

	bool isReg = OTAPI_Wrap::IsNym_RegisteredAtServer(nymID, mServerID);

	if (!isReg)
	{
		string response = madeEasy.register_nym(mServerID, nymID);
		nOT::nUtils::DisplayStringEndl(cout, response);
		nOT::nUtils::DisplayStringEndl(cout, "Nym " + ToStr(nymName) + "(" + ToStr(nymID) + ")" + " was registered successfully");
	}
	else
		cout << "Nym " << nymName << "(" << nymID << ")" << " was already registered" << endl;
}

void cUseOT::nymRegister(const string & nymName, const string & serverName) {
	if(!Init())
	return ;
	//TODO: Make work with servers aliases
}

void cUseOT::nymSetDefault(const string & nymName) {
	if(!Init())
		return ;
	mUserID = nymGetId(nymName);
}

const vector<string> cUseOT::nymsGetMy() {
	if(!Init())	return vector<string> {};

	if (!mNymsMy_loaded) {
		try {
		mNymsMy_loaded=0; // to mark that we start to delete data/data is inconsistent
		//mNymsMy.clear(); // FIXME Not used class cNyminfo (look: othint.cpp)
		mNymsMy_str.clear();

		for(int i = 0 ; i < OTAPI_Wrap::GetNymCount ();i++) {
			string nym_ID = OTAPI_Wrap::GetNym_ID (i);
			string nym_Name = OTAPI_Wrap::GetNym_Name (nym_ID);

			mNymsMy_str.push_back(nym_Name);
		}
	}
	catch(...) { }
	mNymsMy_loaded = true;
	}
return mNymsMy_str;
}

const string cUseOT::serverGetDefault() {
	if(!Init())
		return "";
	return mServerID;
}

const string cUseOT::serverGetId(const string & serverName) { // Gets nym aliases and IDs begins with '%'
	if(!Init())
	return "";

	if (serverName.at(0) == '%') { // nym ID
		return serverName.substr(1);
	}
	else { // nym Name
		for(int i = 0 ; i < OTAPI_Wrap::GetServerCount(); i++) {
			string nymID = OTAPI_Wrap::GetServer_ID(i);
			string serverName_ = OTAPI_Wrap::GetServer_Name(nymID);
			if (serverName_ == serverName)
				return nymID;
		}
	}
	return "";
}

void cUseOT::serverSetDefault(const string & serverName) {
	if(!Init())
		return ;
	mServerID = serverGetId(serverName);
}

const vector<string> cUseOT::serversGet() { ///< Get all servers name
	if(!Init())
	return vector<string> {};

	vector<string> servers;
	for(int i = 0 ; i < OTAPI_Wrap::GetServerCount ();i++) {
		string servID = OTAPI_Wrap::GetServer_ID(i);
		string servName = OTAPI_Wrap::GetServer_Name(servID);
		servers.push_back(servName + " - " + servID);
	}
	return servers;
}

const string cUseOT::textEncode(const string & plainText) {
	if(!Init())
		return "";

	bool bLineBreaks = true; // FIXME? OTAPI_Wrap - bLineBreaks should usually be set to true
	string encodedText;
	encodedText = OTAPI_Wrap::Encode (plainText, bLineBreaks);
	return encodedText;
}

const string cUseOT::textEncrypt(const string & recipientNymName, const string & plainText) {
	if(!Init())
		return "";
	string encryptedText;
	encryptedText = OTAPI_Wrap::Encrypt(nymGetId(recipientNymName), plainText);
	return encryptedText;
}

const string cUseOT::textDecode(const string & encodedText) {
	if(!Init())
		return "";

	bool bLineBreaks = true; // FIXME? OTAPI_Wrap - bLineBreaks should usually be set to true
	string plainText;
	plainText = OTAPI_Wrap::Decode (encodedText, bLineBreaks);
	return plainText;
}

const string cUseOT::textDecrypt(const string & recipientNymName, const string & encryptedText) {
	if(!Init())
		return "";
	string plainText;
	plainText = OTAPI_Wrap::Decrypt(nymGetId(recipientNymName), encryptedText);
	return plainText;
}

cUseOT useOT;
} // nUse
}; // namespace OT

