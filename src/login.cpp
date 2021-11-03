#include "login.h"

//Userinfo

Userinfo::Userinfo(const std::string *ptr_userfile_path){
	try
	{
		//open txtfile
		m_userinfo_txt.open(*ptr_userfile_path, std::ios::in | std::ios::out | std::ios::binary);
		m_userinfo_txt.exceptions(std::ifstream::badbit);


	}
	catch (const std::ifstream::failure &e)
	{
		throw csv::Error(std::string("CTOR: Error accessing Database: ").append(e.what()));
	}
}

bool Userinfo::create_user_info(std::string_view _ID){
	//get necessary info
	m_ID = _ID;

	std::string* ptr_first_name = new std::string;
	std::string* ptr_second_name = new std::string;

	log("Please enter your first name: ");
	std::cin>>*ptr_first_name;
	log("Please enter your second name: ");
	std::cin>>*ptr_second_name;

	m_user_name = *ptr_first_name + "," + *ptr_second_name;

	//write data to file
	m_userinfo_txt.seekg(0, std::ios_base::end);

	m_userinfo_txt <<"\n~";
	int position_place = m_userinfo_txt.tellg();
	m_userinfo_txt <<"=============="<< m_ID <<"======\n";
	m_userinfo_txt <<"Name:"<<m_user_name<<"\n";
	m_userinfo_txt <<"Books:"<<"\n";
	m_userinfo_txt <<"Owned:"<<"\n";
	if(m_ID.at(0) == 'P'){
		m_userinfo_txt <<"Published:\n";
	}

	m_next_position = m_userinfo_txt.tellg();
	m_userinfo_txt.seekg(position_place);
	m_userinfo_txt << m_next_position;
	m_userinfo_txt.flush();
}

//User

User::User(){
	//temporary allocated values
	mptr_username = new std::string;
	mptr_password = new std::string;

	//longer allocation
	mptr_csv_parser = new csv::CSVParser(&m_path_userfile);
	mptr_userinfo = new Userinfo(&m_path_userinfo);
	m_ID = "U";
};

User::~User(){
	delete mptr_csv_parser;
	delete mptr_userinfo;
};

bool User::m_user_request(){
	int _tries = 0;
	while(_tries < 3){
		log("Username>> ");
		std::getline(std::cin, *mptr_username);
		log("Password>> ");
		std::getline(std::cin, *mptr_password);

		for(csv::_HEADER_TYPE i = 0; i < mptr_csv_parser->size(); i++){
			if(mptr_csv_parser->getRow(i)["USERNAME"] == *mptr_username){
				if(mptr_csv_parser->getRow(i)["PASSWORD"] == *mptr_password){
					m_login_flag = true;
					return m_login_flag;
				}
			}
		}

		log("Wrong username or password.");
		++_tries;
	}

	return m_login_flag = false;
};

bool User::m_create_user(){
	csv::Row* _temp_rowptr = new csv::Row();
	std::regex _reg_username{"(?!.*,)(?=[a-zA-Z0-9._]{8,20}$)(?!.*[_.]{2})[^_.].*[^_.]$"};
	std::regex _reg_password{"(?!.*,)(?=.*?[A-Z])(?=.*?[a-z])(?=.*?[0-9])(?=.*?[#?!@$%^&*<>_]).{8,}"};
	
	//Get Username
	do
	{
		log("New Username>> ");
		std::getline(std::cin, *mptr_username);
		while(!std::regex_match(*mptr_username, _reg_username)){
			log("Username must contain 8-20 Characters, start and end with a character,	can contain numbers, '.' and '_'\n");
			log("New Username>> ");
			std::getline(std::cin, *mptr_username);
		}
		
		if(mptr_csv_parser->find_first_of(*mptr_username, "USERNAME")){
			log("Username already exists\n");
			continue;
		}
		else{
			break;
		}
	} while (true);
	_temp_rowptr->add_value(*mptr_username);

	//Get Password
	log("New Password>> ");
	std::getline(std::cin, *mptr_password);
	while (!std::regex_match(*mptr_password, _reg_password))
	{
		log("Password criteria:\n - min. 8 characters ");
		log("\n - One upper one lower case character \n - One number\n - one special character eg. #?!@$%^&*<>_\n");
		log("New Password>> ");
		std::getline(std::cin, *mptr_password);
	}
	_temp_rowptr->add_value(*mptr_password);

	//Create UID
	_temp_rowptr->add_value(m_create_ID(100000,999999));

	//write new User to Userfile and save in Parser
	mptr_csv_parser->addRow(*_temp_rowptr);

	mptr_userinfo->create_user_info(m_ID);
};

std::string_view User::m_create_ID(int min, int max){
		std::uniform_int_distribution<> _distribution(min, max);
		std::random_device _random_dev;
		std::default_random_engine _generator(_random_dev());
		int _rand = _distribution(_generator);
	do
	{
		m_ID += std::to_string(_rand);
		if(mptr_csv_parser->find_first_of(m_ID, "UID")){
			char _token = m_ID.at(0);
			m_ID = _token;
			continue;
		}
		else{
			break;
		}

	} while (true);

	return m_ID;
}

bool User::login(){
	char _yn = 0;
	while (true)
	{
		log("Log into an existing Account? [y/n]\n>>");
		std::cin >> _yn;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (!std::cin.fail())
		{
			switch (_yn)
			{
			case 'y':
				if (!m_user_request())
				{
					log("Do you instead want to create a new Account? [y/n]\n>>");
					std::cin>>_yn;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					if (!std::cin.fail())
					{
						switch (_yn)
						{
						case 'y':
							return m_create_user();
						case 'n':
							log("Login failed.");
							return m_login_flag = false;
						default:
						    log("Login failed.");
							return m_login_flag = false;
						}
					}				
				}
				else {
					//do some logging for user activity
					log("Login successful\n");
					return m_login_flag = true;
				}
				break;
			case 'n':
				return m_create_user();
				break;
			default:
				log("Wrong input. Enter 'y' or 'n'.");
				break;
			}
		}
		else
		{
			log("Wrong input. Enter 'y' or 'n'.");
			std::cin.clear();
		}
	}

	return m_login_flag = false;
};