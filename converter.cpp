
#include "mimalloc-new-delete.h"

#include <iostream>
#include <fstream>
#include <string>

#include "clau_parser.h"

#include <map>
#include <set>


#include <filesystem>


// func - > map1, map2

// ut <-   link= { left = x  right = y }  link = { right = y  left = x }  ...

std::map<std::string, std::vector<std::string>> func(clau_parser::UserType* ut,
	std::string_view left, std::string_view right)
{
	std::map<std::string, std::vector<std::string>> result;

	for (int i = 0; i < ut->GetUserTypeListSize(); ++i) {
		std::pair<std::string, std::string> _pair;
		for (int j = 0; j < ut->GetUserTypeList(i)->GetItemListSize(); ++j) {
			auto x = ut->GetUserTypeList(i)->GetItemList(j);
			if (x.GetName() == left) {
				_pair.first = x.Get();
			}
			else if (x.GetName() == right) {
				_pair.second = x.Get();
			}
		}
		if (result.find(_pair.first) != result.end() && _pair.second.empty() == false) {
			result[_pair.first].push_back(_pair.second);
		}
		else {
			if (!_pair.second.empty()) {
				result[_pair.first] = std::vector<std::string>{ _pair.second };
			}
		}
	}

	return result;
}

std::map<std::string, std::string> func2(clau_parser::UserType* ut,
	const std::string& version,
	std::string_view left, std::string_view right)
{
	std::map<std::string, std::string> result;

	ut = ut->GetUserTypeItem(version)[0];

	for (int i = 0; i < ut->GetUserTypeListSize(); ++i) { // enter 'link = { }'
		
		std::string right_str;
		std::vector<std::string> left_vec;
		
		for (int j = 0; j < ut->GetUserTypeList(i)->GetItemListSize(); ++j) {
			auto x = ut->GetUserTypeList(i)->GetItemList(j);
			if (x.GetName() == left) {
				left_vec.push_back(x.Get());
			}
			else if (x.GetName() == right) {
				right_str = (x.Get());
			}
		}

		for (int i = 0; i < left_vec.size(); ++i) {
			result.insert(std::make_pair(left_vec[i], right_str));
		}
	}

	return result;
}

std::map<std::string, std::string> func3(clau_parser::UserType* ut,
	const std::string& version,
	std::string_view left, std::string_view right)
{
	std::map<std::string, std::string> result;

	ut = ut->GetUserTypeItem(version)[0];

	for (int i = 0; i < ut->GetUserTypeListSize(); ++i) { // enter 'link = { }'

		std::string left_str;
		std::vector<std::string> right_vec;

		for (int j = 0; j < ut->GetUserTypeList(i)->GetItemListSize(); ++j) {
			auto x = ut->GetUserTypeList(i)->GetItemList(j);
			if (x.GetName() == left) {
				left_str = x.Get();
			}
			else if (x.GetName() == right) {
				right_vec.push_back(x.Get());
			}
		}

		for (int i = 0; i < right_vec.size(); ++i) {
			result.insert(std::make_pair(left_str, right_vec[i]));
		}
	}

	return result;
}

std::map<std::string, std::vector<std::string>> chk_merge_nation(std::map<std::string, std::vector<std::string>>& _map, clau_parser::UserType* merge_nation, std::map<std::string, std::vector<std::string>>& eu3_vic2) {
	auto x = merge_nation->GetUserTypeList(0);
	
	_map = eu3_vic2;

	for (int i = 0; i < x->GetUserTypeListSize(); ++i) {
		auto y = x->GetUserTypeList(i);

		auto master = eu3_vic2[y->GetItem("master")[0].Get()];  // eu4 tag
		auto slaves = y->GetItem("slave"); // eu3 tag

		for (auto _ : slaves) {
			if (_map.find(_.Get()) != _map.end()) {
				_map[_.Get()].clear();
				for (auto __ : master) {
					_map[_.Get()].push_back(__); // eu3 tag -> eu4 tag
					std::cout << _.Get() << " -> " << __ << "\n";
				}
			}
		}
	}

	eu3_vic2 = _map;

	return _map;
}

inline std::string GetEU4Country(const std::string& eu3_country, const std::map<std::string, std::vector<std::string>>& eu3_vic2, const  std::map<std::string, std::vector<std::string>>& vic2_eu4) {
	std::string x;

	try {
		auto y = eu3_vic2.at(eu3_country);
		std::vector<std::string> vec;
		for (auto _ : y) {
			auto k = vic2_eu4.at(_);

			for (auto _ : k) {
				if (!_.empty()) {
					vec.push_back(_);
				}
			}
		}

		if (!vec.empty()) {
			x = vec[0];
		}
	}
	catch (...) {
	}


	return x;
}

inline std::string GetEU3Province(const std::string& eu4_country, const std::map<std::string, std::string>& eu4_vic2, const  std::map<std::string, std::string>& vic2_eu3) {
	std::string x;

	try {
		x = vic2_eu3.at(eu4_vic2.at(eu4_country));
	}
	catch (...) {
	}

	return x; 
}

inline std::string GetEU3Province2(const std::string& eu4_country, const std::map<std::string, std::string>& eu4_vic2, const  std::map<std::string, std::string>& vic2_eu3) {
	std::string x;

	try {
		


		x = vic2_eu3.at(eu4_vic2.at(eu4_country));
	}
	catch (...) {
		//
	}

	return x;
}

inline std::string Remove(std::string x) {
	x = x.substr(1, x.size() - 2);
	return x;
}

int main(void)
{

	clau_parser::UserType area;

	clau_parser::LoadData::LoadDataFromFile(L"area.txt", area);

	clau_parser::UserType merge_nations_eu3_vic2;
	int count = 0;

	clau_parser::UserType eu3_vic2_country;
	clau_parser::UserType eu4_vic2_country;

	clau_parser::LoadData::LoadDataFromFile(L"merge_nations_eu3_vic2.txt", merge_nations_eu3_vic2);


	clau_parser::LoadData::LoadDataFromFile(L"country_mappings_eu3_vic2.txt", eu3_vic2_country);
	clau_parser::LoadData::LoadDataFromFile(L"country_mappings_eu4_vic2.txt", eu4_vic2_country);

	std::map<std::string, std::vector<std::string>> eu3_vic_ct;
	{
		auto x = func(&eu3_vic2_country, "eu3", "vic");

		eu3_vic_ct = chk_merge_nation(eu3_vic_ct, &merge_nations_eu3_vic2, x);
	}
	auto y = func(&eu4_vic2_country, "v2", "eu4");
	auto y2 = func(&eu4_vic2_country, "eu4", "v2");

	clau_parser::UserType eu3_vic2_province;
	clau_parser::UserType eu4_vic2_province;

	clau_parser::LoadData::LoadDataFromFile(L"province_mappings_eu3_vic2.txt", eu3_vic2_province);
	clau_parser::LoadData::LoadDataFromFile(L"province_mappings_eu4_vic2.txt", eu4_vic2_province);

	clau_parser::UserType eu3_vic2_religion;
	clau_parser::UserType eu4_vic2_religion;

	clau_parser::LoadData::LoadDataFromFile(L"religionMap.txt", eu3_vic2_religion);
	clau_parser::LoadData::LoadDataFromFile(L"religion_map.txt", eu4_vic2_religion);


	clau_parser::UserType eu3_vic2_culture;
	clau_parser::UserType eu4_vic2_culture;

	clau_parser::LoadData::LoadDataFromFile(L"cultureMap.txt", eu3_vic2_culture);
	clau_parser::LoadData::LoadDataFromFile(L"culture_map.txt", eu4_vic2_culture);



	/// main work.
	clau_parser::UserType eu3save;

	clau_parser::LoadData::LoadDataFromFile(
		//L"C:/Program Files (x86)/Steam/steamapps/common/Europa Universalis III - Complete/save games/Manchu1912_12_08.eu3"
		L"C:/Program Files (x86)/Steam/steamapps/common/Europa Universalis III - Complete/save games/Great_Britain2402_08_16.eu3"
		, eu3save);


	//clau_parser::LoadData::LoadDataFromFile(L"C:/Program Files (x86)/Steam/steamapps/common/Europa Universalis III - Complete/save games/Great_Britain1818_12_17.eu3", eu3save);

	std::map<std::wstring, clau_parser::UserType> eu4_provinces;
	std::map<std::wstring, clau_parser::UserType> eu4_countries;

	std::map<std::string, std::wstring> eu3_eu4_provinces;

	std::map<std::wstring, std::string> eu4_countries_primary_culture; // primary_culture
	std::map<std::wstring, std::string> eu4_countries_primary_religion; // primary_religion

	{
		for (auto& p : std::filesystem::directory_iterator("provinces")) { // /provinces/

			//std::cout << p.path() << "\n";

			clau_parser::UserType eu4_province;

			clau_parser::LoadData::LoadDataFromFile(p.path(), eu4_province);
			std::wstring eu4_province_name = p.path();

			{
				auto x = eu4_province_name.rfind('/');
				if (x != std::wstring::npos) {
					eu4_province_name = eu4_province_name.substr(x + 1);
				}
				x = eu4_province_name.rfind('\\');
				if (x != std::wstring::npos) {
					eu4_province_name = eu4_province_name.substr(x + 1);
				}
			}

			{
				auto y = eu4_province_name.find(' ');
				if (y != std::string::npos) {
					eu4_province_name = eu4_province_name.substr(0, y);
				}

				auto x = eu4_province_name.find('-');
				if (x != std::string::npos) {
					eu4_province_name = eu4_province_name.substr(0, x);
				}
			}

			//std::wcout << L"eu4 province name " << eu4_province_name << L"\n";
			std::vector<clau_parser::UserType*> arr;

			{
				std::string key = std::to_string(std::stoi(eu4_province_name));

				std::vector<std::string> vec;

				auto x = eu4_vic2_province.GetUserTypeItem("1.31.0.0")[0];
				bool pass = false;
				for (int i = 0; i < x->GetUserTypeListSize(); ++i) {
					auto y = x->GetUserTypeList(i);
					for (int j = 0; j < y->GetItemListSize(); ++j) {
						if (y->GetItemList(j).GetName() == "eu4" &&
							y->GetItemList(j).Get() == key) {

							for (int k = 0; k < y->GetItemListSize(); ++k) {
								if (y->GetItemList(k).GetName() == "v2") {
									vec.push_back(y->GetItemList(k).Get());
								}
							}

							pass = true;
						}
					}
				}

				if (pass) {
					auto a = eu3_vic2_province.GetUserTypeItem("dw")[0];
					bool pass = false;

					for (int q = 0; q < vec.size(); ++q) {

						const std::string key = vec[q];

						for (int i = 0; i < a->GetUserTypeListSize(); ++i) {
							auto y = a->GetUserTypeList(i);
							for (int j = 0; j < y->GetItemListSize(); ++j) {
								if (y->GetItemList(j).GetName() == "vic" &&
									y->GetItemList(j).Get() == key) {

									for (int k = 0; k < y->GetItemListSize(); ++k) {
										if (y->GetItemList(k).GetName() == "eu3") {
											auto _arr = eu3save.GetUserTypeItem(y->GetItemList(k).Get());

											for (int i = 0; i < _arr.size(); ++i) {
												arr.push_back(_arr[i]);
											}
											pass = true;
										}
									}
								}
							}
						}
					}
				}
				else {

					std::cout << "eee\n";
					//	return -2;
				}
			}


			bool chk = false;


			for (int i = 0; i < arr.size(); ++i) {
				if (chk) {
					break;
				}

				clau_parser::UserType* eu3_province = arr[i];

				//std::cout << "eu3 province " << eu3_province->GetName() << "\n";

				eu3_eu4_provinces[eu3_province->GetName()] = eu4_province_name;
				//break;
			}
			if (arr.empty()) {
				//
			}

		}
	}

	{
		for (auto& p : std::filesystem::directory_iterator("countries")) { // /countries/

			//std::cout << p.path() << "\n";

			clau_parser::UserType eu4_country;

			clau_parser::LoadData::LoadDataFromFile(p.path(), eu4_country);
			std::wstring eu4_country_name = p.path();

			{
				auto x = eu4_country_name.rfind('/');
				if (x != std::wstring::npos) {
					eu4_country_name = eu4_country_name.substr(x + 1);
				}
				x = eu4_country_name.rfind('\\');
				if (x != std::wstring::npos) {
					eu4_country_name = eu4_country_name.substr(x + 1);
				}
			}

			{
				auto y = eu4_country_name.find(' ');
				if (y != std::string::npos) {
					eu4_country_name = eu4_country_name.substr(0, y);
				}

				auto x = eu4_country_name.find('-');
				if (x != std::string::npos) {
					eu4_country_name = eu4_country_name.substr(0, x);
				}
			}

			std::vector<clau_parser::UserType*> arr;

			{
				std::string key; key.reserve(eu4_country_name.size());
				for (int i = 0; i < eu4_country_name.size(); ++i) {
					key.push_back(eu4_country_name[i]);
				}
				std::vector<std::string> vec;

				auto& x = eu4_vic2_country;
				bool pass = false;
				for (int i = 0; i < x.GetUserTypeListSize(); ++i) {
					auto& y = x.GetUserTypeList(i);
					for (int j = 0; j < y->GetItemListSize(); ++j) {
						if (y->GetItemList(j).GetName() == "eu4" &&
							y->GetItemList(j).Get() == key) {

							for (int k = 0; k < y->GetItemListSize(); ++k) {
								if (y->GetItemList(k).GetName() == "v2") {
									vec.push_back(y->GetItemList(k).Get());
								}
							}

							pass = true;
						}
					}
				}

				if (pass) {
					auto& a = eu3_vic2_country;
					bool pass = false;

					for (int q = 0; q < vec.size(); ++q) {

						const std::string key = vec[q];

						for (int i = 0; i < a.GetUserTypeListSize(); ++i) {
							auto y = a.GetUserTypeList(i);
							for (int j = 0; j < y->GetItemListSize(); ++j) {
								if (y->GetItemList(j).GetName() == "vic" &&
									y->GetItemList(j).Get() == key) {

									for (int k = 0; k < y->GetItemListSize(); ++k) {
										if (y->GetItemList(k).GetName() == "eu3") {
											auto _arr = eu3save.GetUserTypeItem(y->GetItemList(k).Get());

											for (int i = 0; i < _arr.size(); ++i) {
												arr.push_back(_arr[i]);
											}
											pass = true;
										}
									}

									if (pass == false) {
										exit(-11);
									}
								}
							}
						}
					}
				}
				else {

					std::cout << "eee\n";
					//	return -2;
				}
			}


			bool chk = false;


			for (int i = 0; i < arr.size(); ++i) {
				if (chk) {
					break;
				}

				clau_parser::UserType* eu3_country = arr[i];

				std::cout << "eu3 country [ "  << i << " ] " << eu3_country->GetName() << "\n";

				{
					eu4_country.RemoveUserTypeList();

					std::string eu3_capital;
					std::string eu4_capital;

					// set capital of country.
					if (auto idx = eu3_country->GetItemIdx("capital"); !idx.empty()) {
						eu3_capital = eu3_country->GetItemList(idx[0]).Get();
						auto temp = eu3_eu4_provinces[eu3_capital];

						for (int i = 0; i < temp.size(); ++i) {
							eu4_capital.push_back(temp[i]);
						}

						std::cout << eu3_capital << " and " << eu4_capital << "\n";

						if (auto idx = eu4_country.GetItemIdx("capital"); !idx.empty()) {
							eu4_country.SetItem(idx[0], eu4_capital);
						}
						else {
							eu4_country.AddItem("capital", eu4_capital);
						}

						// fixed_capital?
					}

					std::cout << "eu4 country name ";
					std::wcout << eu4_country_name;
					std::cout << "\n";
					eu4_countries[eu4_country_name] = eu4_country;

					if (auto idx = eu4_country.GetItemIdx("primary_culture"); !idx.empty()) {
						eu4_countries_primary_culture[eu4_country_name] = eu4_country.GetItemList(idx[0]).Get();
						std::cout << "culture : " << eu4_countries_primary_culture[eu4_country_name] << "\n";
					}
					if (auto idx = eu3_country->GetItemIdx("religion"); !idx.empty()) {
						eu4_countries_primary_religion[eu4_country_name] = eu3_country->GetItemList(idx[0]).Get();
						std::cout << "religion : " << eu4_countries_primary_religion[eu4_country_name] << "\n";
					}
					//clau_parser::LoadData::Save(eu4_province, std::wstring(L"history/provinces/") + eu4_province_name + L".txt");
				}

				//break;
			}
		}
	}

	{
		for (auto& p : std::filesystem::directory_iterator("provinces")) { // /provinces/

			//std::cout << p.path() << "\n";

			clau_parser::UserType eu4_province;

			clau_parser::LoadData::LoadDataFromFile(p.path(), eu4_province);
			std::wstring eu4_province_name = p.path();

			{
				auto x = eu4_province_name.rfind('/');
				if (x != std::wstring::npos) {
					eu4_province_name = eu4_province_name.substr(x + 1);
				}
				x = eu4_province_name.rfind('\\');
				if (x != std::wstring::npos) {
					eu4_province_name = eu4_province_name.substr(x + 1);
				}
			}

			{
				auto y = eu4_province_name.find(' ');
				if (y != std::string::npos) {
					eu4_province_name = eu4_province_name.substr(0, y);
				}

				auto x = eu4_province_name.find('-');
				if (x != std::string::npos) {
					eu4_province_name = eu4_province_name.substr(0, x);
				}
			}

			//std::wcout << L"eu4 province name " << eu4_province_name << L"\n";
			std::vector<clau_parser::UserType*> arr;

			{
				std::string key = std::to_string(std::stoi(eu4_province_name));

				std::vector<std::string> vec;

				auto x = eu4_vic2_province.GetUserTypeItem("1.31.0.0")[0];
				bool pass = false;
				for (int i = 0; i < x->GetUserTypeListSize(); ++i) {
					auto y = x->GetUserTypeList(i);
					for (int j = 0; j < y->GetItemListSize(); ++j) {
						if (y->GetItemList(j).GetName() == "eu4" &&
							y->GetItemList(j).Get() == key) {

							for (int k = 0; k < y->GetItemListSize(); ++k) {
								if (y->GetItemList(k).GetName() == "v2") {
									vec.push_back(y->GetItemList(k).Get());
								}
							}

							pass = true;
						}
					}
				}

				if (pass) {
					auto a = eu3_vic2_province.GetUserTypeItem("dw")[0];
					bool pass = false;

					for (int q = 0; q < vec.size(); ++q) {

						const std::string key = vec[q];

						for (int i = 0; i < a->GetUserTypeListSize(); ++i) {
							auto y = a->GetUserTypeList(i);
							for (int j = 0; j < y->GetItemListSize(); ++j) {
								if (y->GetItemList(j).GetName() == "vic" &&
									y->GetItemList(j).Get() == key) {

									for (int k = 0; k < y->GetItemListSize(); ++k) {
										if (y->GetItemList(k).GetName() == "eu3") {
											auto _arr = eu3save.GetUserTypeItem(y->GetItemList(k).Get());

											for (int i = 0; i < _arr.size(); ++i) {
												arr.push_back(_arr[i]);
											}
											pass = true;
										}
									}
								}
							}
						}
					}
				}
				else {

					std::cout << "eee\n";
					//	return -2;
				}
			}


			bool chk = false;


			for (int i = 0; i < arr.size(); ++i) {
				if (chk) {
					break;
				}

				clau_parser::UserType* eu3_province = arr[i];

				std::cout << "eu3 province " << eu3_province->GetName() << "\n";

				{
					eu4_province.RemoveUserTypeList();

					if (!eu4_province.GetItemIdx("is_city").empty()) {
						eu4_province.SetItem("is_city", "no");
					}

					// owner, controller are eu4 contry tag.
				//	std::string capital = GetEU4Country(Remove(eu3_province->GetItem("capital")[0].Get()), x, y);

					/*
					if (!eu3_province->GetItem("citysize").empty() && !eu3_province->GetItem("citysize").empty() && !eu3_province->GetItem("manpower").empty()) {
						if (!eu4_province.GetItemIdx("base_tax").empty()) {
							eu4_province.SetItem("base_tax", std::to_string((int)(std::stod(eu3_province->GetItem("base_tax")[0].Get()) + std::stod(eu3_province->GetItem("citysize")[0].Get()) / 50000 + 1)));
						}
						if (!eu4_province.GetItemIdx("base_production").empty()) {
							eu4_province.SetItem("base_production", std::to_string((int)(std::stod(eu3_province->GetItem("citysize")[0].Get()) / 50000 + std::stod(eu3_province->GetItem("citysize")[0].Get()) / 50000 + 1)));
						}
						if (!eu4_province.GetItemIdx("base_manpower").empty()) {
							eu4_province.SetItem("base_manpower", std::to_string((int)(std::stod(eu3_province->GetItem("manpower")[0].Get()) + std::stod(eu3_province->GetItem("citysize")[0].Get()) / 50000 + 1)));
						}
					}*/
					std::wstring _owner; std::string owner;
					if (!eu3_province->GetItemIdx("owner").empty()) {

						//	eu4_province.AddItem("pass", "true");


						owner = GetEU4Country(Remove(eu3_province->GetItem("owner")[0].Get()), eu3_vic_ct, y);
						for (int i = 0; i < owner.size(); ++i) {
							_owner.push_back(owner[i]);
						}
						std::cout << owner << " | ";
						if (owner.empty() == false) {
							if (eu4_province.GetItemIdx("owner").empty()) {
								eu4_province.AddItem("owner", "");
							}count++;

							chk = true;

							eu4_province.GetItemList(eu4_province.GetItemIdx("owner")[0]).Set(0, owner);

							eu4_province.AddUserTypeItem(clau_parser::UserType("1510.1.1"));

							eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("owner", owner);
							eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("base_tax", owner);
							eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("base_production", owner);
							eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("base_manpower", owner);


							eu4_province.AddItem("add_core", owner); // add_territorial_core
							if (eu4_province.GetItem("is_city").empty()) {
								eu4_province.AddItem("is_city", "yes");
							}
							else {
								eu4_province.SetItem("is_city", "yes");
							}
						}
						else {
							std::cout << "chked...1\n";
						}
					}

					if (!eu3_province->GetItemIdx("controller").empty()) {
						std::string controller = GetEU4Country(Remove(eu3_province->GetItem("controller")[0].Get()), eu3_vic_ct, y);

						if (controller.empty() == false) {
							if (eu4_province.GetItemIdx("controller").empty()) {
								eu4_province.AddItem("controller", "");
							}
							eu4_province.GetItemList(eu4_province.GetItemIdx("controller")[0]).Set(0, controller);

							if (eu4_province.GetItem("is_city").empty()) {
								eu4_province.AddItem("is_city", "yes");
							}
							else {
								eu4_province.SetItem("is_city", "yes");
							}
						}
					}
					//eu4_province.GetItemList(eu4_province.GetItemIdx("capital")[0]).Set(0, capital);

					eu4_province.AddItem("discovered_by", "eastern");
					eu4_province.AddItem("discovered_by", "western");
					eu4_province.AddItem("discovered_by", "muslim");
					eu4_province.AddItem("discovered_by", "ottoman");


					eu4_province.SetName(std::to_string(std::stoi(eu4_province_name))); // ?


					if (eu3_province->GetItem("religion").empty() == false) {
						std::string eu4_religion;
						std::string vic2_religion;
						std::string eu3_religion = eu3_province->GetItem("religion")[0].Get();
						auto& eu3 = eu3_vic2_religion.GetUserTypeList(0);

						bool pass = false;
						for (int i = 0; i < eu3->GetUserTypeListSize(); ++i) {
							// link = { 
							auto x = eu3->GetUserTypeList(i)->GetItem("eu3");
							for (int j = 0; j < x.size(); ++j) {
								if (x[j].Get() == eu3_religion) {
									auto y = eu3->GetUserTypeList(i)->GetItem("vic")[0];
									vic2_religion = y.Get();
									pass = true;
									break;
								}
							}
							if (pass) {
								break;
							}
						}

						pass = false;
						auto& eu4 = eu4_vic2_religion;
						for (int i = 0; i < eu4.GetUserTypeListSize(); ++i) {
							auto x = eu4.GetUserTypeList(i)->GetItem("v2");
							for (int j = 0; j < x.size(); ++j) {
								if (x[j].Get() == vic2_religion) {
									auto y = eu4.GetUserTypeList(i)->GetItem("eu4")[0];
									eu4_religion = y.Get();
									pass = true;
									break;
								}
							}
							if (pass) {
								break;
							}
						}

						if (_owner.empty() == false) { //if ((!pass && _owner.empty() == false) || (eu4_culture == "" && _owner.empty() == false)) {
							std::wcout << "pass " << pass << " owner " << _owner << "\n";
							eu4_religion = eu4_countries_primary_religion[_owner];
							if (eu4_religion == "") {
								std::cout << "empty ... \n";
								exit(-1);
							}
							std::cout << "religion " << eu4_religion << "\n";
						}


						//std::cout << "eu4_religion : " << eu4_religion << "\n";
						if (eu4_province.GetItem("religion").empty()) {
							eu4_province.AddItem("religion", eu4_religion);
						}
						else {
							eu4_province.SetItem("religion", eu4_religion);
						}

						if (!owner.empty()) {
							eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("religion", eu4_religion);
						}
					}
					if (eu3_province->GetItem("culture").empty() == false) {
						std::string eu4_culture;
						std::string vic2_culture;
						std::string eu3_culture = eu3_province->GetItem("culture")[0].Get();
						auto& eu3 = eu3_vic2_culture.GetUserTypeList(0);

						bool pass = false;
						for (int i = 0; i < eu3->GetUserTypeListSize(); ++i) {
							// link = { 
							auto x = eu3->GetUserTypeList(i)->GetItem("eu3");
							for (int j = 0; j < x.size(); ++j) {
								if (x[j].Get() == eu3_culture) {
									auto y = eu3->GetUserTypeList(i)->GetItem("vic")[0];
									vic2_culture = y.Get();
									pass = true;
									break;
								}
							}
							if (pass) {
								break;
							}
						}

						pass = false;
						auto& eu4 = eu4_vic2_culture;
						for (int i = 0; i < eu4.GetUserTypeListSize(); ++i) {
							auto x = eu4.GetUserTypeList(i)->GetItem("vic2");
							for (int j = 0; j < x.size(); ++j) {
								if (x[j].Get() == vic2_culture) {
									auto y = eu4.GetUserTypeList(i)->GetItem("eu4")[0];

									if (y.Get() == "crusader_culture") {
										continue;
									}
									if (y.Get() == "acadian") {
										continue;
									}

									eu4_culture = y.Get();

									pass = true;
									// last one?
								}
							}
							if (pass) {
								break;
							}
						}

						std::cout << eu4_culture << "\n";
						if (_owner.empty() == false) { //if ((!pass && _owner.empty() == false) || (eu4_culture == "" && _owner.empty() == false)) {
							std::wcout << "pass " << pass << " owner " << _owner << "\n";
							eu4_culture = eu4_countries_primary_culture[_owner];

							if (eu4_culture == "") {
								std::cout << "chk";
								exit(-1);
							}


							std::cout << "culture " << eu4_culture << "\n";
						}

						if (eu4_culture == "No Culture") {
							std::cout << "err";
							exit(-2);
						}

						if (eu4_culture.empty() == false) {
							if (eu4_province.GetItem("culture").empty()) {
								eu4_province.AddItem("culture", eu4_culture);
							}
							else {
								eu4_province.SetItem("culture", eu4_culture);
							}
						}

						if (!owner.empty()) {
							eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("culture", eu4_culture);
						}
					}

					

					eu4_provinces[eu4_province_name] = eu4_province;
					//clau_parser::LoadData::Save(eu4_province, std::wstring(L"history/provinces/") + eu4_province_name + L".txt");
				}


				eu3_eu4_provinces[eu3_province->GetName()] = eu4_province_name;
				//break;
			}
			if (arr.empty()) {
				//
			}

		}
	}

	for (int i = 0; i < area.GetUserTypeListSize(); ++i) {
		auto x = area.GetUserTypeList(i);

		std::map<std::string, std::tuple<int, std::string, std::string>> _map;

		for (int j = 0; j < x->GetItemListSize(); ++j) {
			auto y = x->GetItemList(j);
			
			auto& eu4_province = eu4_provinces[std::to_wstring(std::stoi(y.Get()))]; // eu4_province
			
			// 
			if (eu4_province.GetItem("owner").empty()) {
				continue;
			}

			std::cout << " expand... owner [ " << j << " ] " << eu4_province.GetItem("owner")[0].Get() << "\n";
			// count number of owner`s provinces in this area.
			if (_map.find(eu4_province.GetItem("owner")[0].Get()) != _map.end()) {
				std::get<0>(_map[eu4_province.GetItem("owner")[0].Get()])++;
			}
			else {
				std::string religion, culture;

				std::string owner = eu4_province.GetItem("owner")[0].Get();
				std::wstring _owner; 
				_owner.append(owner.begin(), owner.end());
				std::wcout << _owner << "\n";
				if (eu4_province.GetItem("religion").empty() == false) {
					religion = eu4_province.GetItem("religion")[0].Get();
				}
				if (eu4_province.GetItem("culture").empty() == false) {
					culture = eu4_province.GetItem("culture")[0].Get();
				}

				if (_owner.empty() == false) { //if ((!pass && _owner.empty() == false) || (eu4_culture == "" && _owner.empty() == false)) {
					if (eu4_countries_primary_religion.find(_owner) != eu4_countries_primary_religion.end()) {
						religion = eu4_countries_primary_religion[_owner];
						std::cout << "chkddd";
					}
					else {
						//exit(-1);
					}
					std::cout << "religion " << religion << "\n";
				}
				if (_owner.empty() == false) { //if ((!pass && _owner.empty() == false) || (eu4_culture == "" && _owner.empty() == false)) {
					
					if (eu4_countries_primary_culture.find(_owner) != eu4_countries_primary_culture.end()) {
						culture = eu4_countries_primary_culture[_owner];
					}
					std::cout << "culture " << culture << "\n";
				}

				_map.insert(std::make_pair(eu4_province.GetItem("owner")[0].Get(), std::make_tuple(0, religion, culture)));
			}
		}

		std::string winner;
		std::string winner_religion;
		std::string winner_culture;
		int val = -1;

		// 모든 프로빈스의 중요도(가치?)를 동등하게 생각해서 // 개수가 많은 쪽이 모든 province를 가진다.?
		for (const auto& _ : _map) {
			if (val < std::get<0>(_.second)) { 
				val = std::get<0>(_.second);
				winner = _.first;
				winner_religion = std::get<1>(_.second);
				winner_culture = std::get<2>(_.second);
			}
		}

		if (val != -1) { // if not 'totally empty area'.
			
			// find empty province in this area.
			for (int j = 0; j < x->GetItemListSize(); ++j) {
				std::cout << "win " << winner << " " << winner_culture << "\n";

				auto y = x->GetItemList(j);

				auto& eu4_province = eu4_provinces[std::to_wstring(std::stoi(y.Get()))]; // eu4_province

				if (eu4_province.GetItem("owner").empty()) {
					
					eu4_province.AddUserTypeItem(clau_parser::UserType("1510.1.1"));

					eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("owner", winner);
					eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("controller", winner);
					eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("religion", winner_religion);
					eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("culture", winner_culture);

					eu4_province.AddItem("owner", winner);
					eu4_province.AddItem("add_core", winner); // add_territorial_core
					count++;
					
					if (eu4_province.GetItem("is_city").empty()) {
						eu4_province.AddItem("is_city", "yes");
					}
					else {
						eu4_province.SetItem("is_city", "yes");
					}
				}

				if (eu4_province.GetItem("controller").empty()) {
					eu4_province.AddItem("controller", winner);
				}
				else {
					eu4_province.SetItem("controller", winner);
				}

				if (eu4_province.GetItem("religion").empty()) {
					eu4_province.AddItem("religion", winner_religion);
				}
				else {
					eu4_province.SetItem("religion", winner_religion);
				}

				if (winner_culture == "No Culture") {
					exit(-3);
				}
				if (winner_culture.empty()) {
					exit(-1);
				}
				if (eu4_province.GetItem("culture").empty()) {
					eu4_province.AddItem("culture", winner_culture);
				}
				else {
					eu4_province.SetItem("culture", winner_culture);
				}

				eu4_province.AddItem("discovered_by", "eastern");
				eu4_province.AddItem("discovered_by", "western");
				eu4_province.AddItem("discovered_by", "muslim");
				eu4_province.AddItem("discovered_by", "ottoman");

			}
		}
		else {
			std::cout << "chk --3" << "\n";
			for (int j = 0; j < x->GetItemListSize(); ++j) {
				auto y = x->GetItemList(j);

				auto& eu4_province = eu4_provinces[std::to_wstring(std::stoi(y.Get()))]; // eu4_province

				eu4_province.AddItem("discovered_by", "eastern");
				eu4_province.AddItem("discovered_by", "western");
				eu4_province.AddItem("discovered_by", "muslim");
				eu4_province.AddItem("discovered_by", "ottoman");
			}
		}
	}

	std::cout << eu4_provinces.size() << "\n";

	for (auto& x : eu4_provinces) {
		auto eu4_province = x.second;
		auto eu4_province_name = x.first;
		

		clau_parser::LoadData::Save(eu4_province, std::wstring(L"history/provinces/") + eu4_province_name + L".txt");
	}

	std::cout << "\n" << "count " << count << "\n";

	//
	{
		
		{
			std::cout << eu4_countries.size() << "\n";

			for (auto& x : eu4_countries) {
				auto eu4_country = x.second;
				auto eu4_country_name = x.first;


				clau_parser::LoadData::Save(eu4_country, std::wstring(L"history/countries/") + eu4_country_name + L".txt");
			}

			std::cout << "\n" << "count " << count << "\n";
		}
	}

	return 0;
}

