//#include "mimalloc-new-delete.h"

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
			auto k = vic2_eu4.find(_);

			if (k == vic2_eu4.end()) {
				continue;
			}

			for (auto _ = k; k != vic2_eu4.end(); ++k) {
				if (!_->second.empty()) {
					for (auto x : _->second) {
						vec.push_back(x);
					}
				}
			}
		}

		if (!vec.empty()) {
			x = vec[0];
		}
	}
	catch (...) {
		std::cout << "map error \n";
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

	clau_parser::LoadData::LoadDataFromFile(L"eu4/area.txt", area);

	clau_parser::UserType merge_nations_eu3_vic2;
	int count = 0;

	clau_parser::UserType eu3_vic2_country;
	clau_parser::UserType eu4_vic2_country;

	clau_parser::LoadData::LoadDataFromFile(L"eu3/merge_nations.txt", merge_nations_eu3_vic2);

	clau_parser::LoadData::LoadDataFromFile(L"eu3/country_mappings.txt", eu3_vic2_country);
	clau_parser::LoadData::LoadDataFromFile(L"eu4/country_mappings.txt", eu4_vic2_country);

	std::map<std::string, std::vector<std::string>> eu3_vic_ct;
	{
		auto x = func(eu3_vic2_country.GetUserTypeList(0), "eu3", "vic");

		eu3_vic_ct = chk_merge_nation(eu3_vic_ct, &merge_nations_eu3_vic2, x);
	}
	auto y = func(&eu4_vic2_country, "v2", "eu4");
	auto y2 = func(&eu4_vic2_country, "eu4", "v2");

	clau_parser::UserType eu3_vic2_province;
	clau_parser::UserType eu4_vic2_province;

	clau_parser::LoadData::LoadDataFromFile(L"eu3/province_mappings.txt", eu3_vic2_province);
	clau_parser::LoadData::LoadDataFromFile(L"eu4/province_mappings.txt", eu4_vic2_province);

	clau_parser::UserType eu3_vic2_religion;
	clau_parser::UserType eu4_vic2_religion;

	clau_parser::LoadData::LoadDataFromFile(L"eu3/religionMap.txt", eu3_vic2_religion);
	clau_parser::LoadData::LoadDataFromFile(L"eu4/religion_map.txt", eu4_vic2_religion);

	clau_parser::UserType eu3_vic2_culture;
	clau_parser::UserType eu4_vic2_culture;

	clau_parser::LoadData::LoadDataFromFile(L"eu3/cultureMap.txt", eu3_vic2_culture);
	clau_parser::LoadData::LoadDataFromFile(L"eu4/culture_map.txt", eu4_vic2_culture);



	/// main work.
	clau_parser::UserType eu3save;

	clau_parser::LoadData::LoadDataFromFile(
		L"C:/Program Files (x86)/Steam/steamapps/common/Europa Universalis III - Complete/save games/input.eu3"
		, eu3save);

	std::map<std::wstring, clau_parser::UserType> eu4_provinces;
	std::map<std::wstring, clau_parser::UserType> eu4_countries;

	std::map<std::string, std::wstring> eu3_eu4_provinces;

	std::map<std::wstring, std::string> eu4_countries_primary_culture; // primary_culture
	std::map<std::wstring, std::string> eu4_countries_primary_religion; // primary_religion



	// EU3 테크 레벨 (국가별)
	std::map<std::wstring, int> eu4_countries_land_tech;
	std::map<std::wstring, int> eu4_countries_naval_tech;
	std::map<std::wstring, int> eu4_countries_trade_tech;
	std::map<std::wstring, int> eu4_countries_production_tech;
	std::map<std::wstring, int> eu4_countries_government_tech;

	int max_land = 72;
	int max_naval = 72;
	int max_trade = 72;
	int max_prod = 72;
	int max_gov = 72;

	{
		for (auto& p : std::filesystem::directory_iterator("eu4/history/provinces")) { // /provinces/

			std::cout << p.path() << "\n";

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
				}
			}

			bool chk = false;

			for (int i = 0; i < arr.size(); ++i) {
				if (chk) {
					break;
				}

				clau_parser::UserType* eu3_province = arr[i];

				eu3_eu4_provinces[eu3_province->GetName()] = eu4_province_name;
			}
			if (arr.empty()) {
				//return -1;
			}

		}
	}

	{
		for (auto& p : std::filesystem::directory_iterator("eu4/history/countries")) { // /countries/

			std::cout << p.path() << "\n";

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

						// 이전 코드 기준: GetUserTypeList(0)->GetUserTypeList(i)
						for (int i = 0; i < a.GetUserTypeList(0)->GetUserTypeListSize(); ++i) {
							auto y = a.GetUserTypeList(0)->GetUserTypeList(i);
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
					clau_parser::LoadData::Save(eu4_country, std::wstring(L"output/history/countries/") + eu4_country_name + L".txt");

					std::cout << "eee\n";
				}
			}

			bool chk = false;

			for (int i = 0; i < arr.size(); ++i) {
				if (chk) {
					break;
				}

				clau_parser::UserType* eu3_country = arr[i];

				std::cout << "eu3 country [ " << i << " ] " << eu3_country->GetName() << "\n";

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

					// EU3 테크 레벨 읽기
					// technology = { land_tech={레벨 투자} naval_tech={레벨 투자} ... }
					auto tech_blocks = eu3_country->GetUserTypeItem("technology");
					if (!tech_blocks.empty()) {
						auto* tech = tech_blocks[0];

						auto readTech = [&](const std::string& name) -> int {
							auto block = tech->GetUserTypeItem(name);
							if (!block.empty() && block[0]->GetIListSize() > 0) {
								try { return std::stoi(block[0]->GetItemList(0).Get()); }
								catch (...) {}
							}
							return 0;
							};

						auto chk = [&](const int tech, int& max)->int {
							if (tech > max) {
								max = tech;
							}
							return tech;
							};

						eu4_countries_land_tech[eu4_country_name] = chk(readTech("land_tech"), max_land);
						eu4_countries_naval_tech[eu4_country_name] = chk(readTech("naval_tech"), max_naval);
						eu4_countries_trade_tech[eu4_country_name] = chk(readTech("trade_tech"), max_trade);
						eu4_countries_production_tech[eu4_country_name] = chk(readTech("production_tech"), max_prod);
						eu4_countries_government_tech[eu4_country_name] = chk(readTech("government_tech"), max_gov);
					}
				}
			}
		}
	}

	{
		for (auto& p : std::filesystem::directory_iterator("eu4/history/provinces")) { // /provinces/

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
				}
			}

			bool chk = false;

			for (int i = 0; i < arr.size(); ++i) {
				if (chk) {
					break;
				}

				clau_parser::UserType* eu3_province = arr[i];

				std::cout << "eu3 province " << eu3_province->GetName() << "\n";
				//std::cout << eu3_province->ToString() << "\n";
				{
					eu4_province.RemoveUserTypeList();

					if (!eu4_province.GetItemIdx("is_city").empty()) {
						//eu4_province.SetItem("is_city", "no");
					}

					// ---- EU3 건물 → EU4 base_tax / base_production / base_manpower 변환 ----
			// 실제 buildings.txt 기준
			// {tax, production, manpower}

			// 계단식 구조 (최고 레벨만 세이브에 저장됨 → 하위 누적 적용)
			// Government:   temple → courthouse → spy_agency → town_hall → college → cathedral
			// Land:         armory → training_fields → barracks → regimental_camp → arsenal → conscription_center
			// Naval:        dock → drydock → shipyard → grand_shipyard → naval_arsenal → naval_base
			// Production:   constable → workshop → counting_house → treasury_office → mint → stock_exchange
			// Trade:        marketplace → trade_depot → canal → road_network → post_office → customs_house
			// Fort:         fort1 → fort2 → fort3 → fort4 → fort5 → fort6
			// Manufactory:  refinery, wharf, weapons, textile, fine_arts_academy, university (독립 건물)
			// One-per-country: tax_assessor, embassy, glorious_monument, march, grain_depot,
			//                  royal_palace, war_college, admiralty (무시)

					static const std::map<std::string, std::tuple<int, int, int>> building_bonus = {
						// Government (세금 관련)
						{ "temple",              {1, 0, 0} },
						{ "courthouse",          {1, 0, 0} },
						{ "spy_agency",          {1, 0, 0} },
						{ "town_hall",           {2, 0, 0} },
						{ "college",             {1, 0, 0} },
						{ "cathedral",           {1, 0, 0} },
						// Land (병력 관련)
						{ "armory",              {0, 0, 1} },
						{ "training_fields",     {0, 0, 1} },
						{ "barracks",            {0, 0, 1} },
						{ "regimental_camp",     {0, 0, 2} },
						{ "arsenal",             {0, 0, 2} },
						{ "conscription_center", {0, 0, 3} },
						// Naval (생산 관련)
						{ "dock",                {0, 1, 0} },
						{ "drydock",             {0, 1, 0} },
						{ "shipyard",            {0, 1, 0} },
						{ "grand_shipyard",      {0, 1, 0} },
						{ "naval_arsenal",       {0, 2, 0} },
						{ "naval_base",          {0, 2, 0} },
						// Production (세금+생산)
						{ "constable",           {1, 0, 0} },
						{ "workshop",            {1, 0, 0} },
						{ "counting_house",      {0, 1, 0} },
						{ "treasury_office",     {1, 0, 0} },
						{ "mint",                {1, 0, 0} },
						{ "stock_exchange",      {2, 0, 0} },
						// Trade (생산)
						{ "marketplace",         {0, 1, 0} },
						{ "trade_depot",         {0, 1, 0} },
						{ "canal",               {0, 1, 0} },
						{ "road_network",        {0, 1, 0} },
						{ "post_office",         {0, 1, 0} },
						{ "customs_house",       {0, 1, 0} },
						// Forts (병력)
						{ "fort1",               {0, 0, 1} },
						{ "fort2",               {0, 0, 1} },
						{ "fort3",               {0, 0, 1} },
						{ "fort4",               {0, 0, 1} },
						{ "fort5",               {0, 0, 2} },
						{ "fort6",               {0, 0, 2} },
						// Manufactories (독립 건물 - 레벨 없음)
						{ "refinery",            {0, 2, 0} },
						{ "wharf",               {0, 2, 0} },
						{ "weapons",             {0, 0, 2} },
						{ "textile",             {0, 2, 0} },
						{ "fine_arts_academy",   {1, 0, 0} },
						{ "university",          {1, 1, 0} },
					};

					// 헬퍼 람다: 건물이 yes인지 확인
					auto hasBuilding = [&](const std::string& name) -> bool {
						auto items = eu3_province->GetItem(name);
						return !items.empty() && items[0].Get() == "yes";
						};

					int bonus_tax = 0, bonus_prod = 0, bonus_man = 0;

					// ── Government 계단식: temple → courthouse → spy_agency → town_hall → college → cathedral
					if (hasBuilding("cathedral")) { bonus_tax += 1 + 1 + 1 + 2 + 1 + 1; }
					else if (hasBuilding("college")) { bonus_tax += 1 + 1 + 1 + 2 + 1; }
					else if (hasBuilding("town_hall")) { bonus_tax += 1 + 1 + 1 + 2; }
					else if (hasBuilding("spy_agency")) { bonus_tax += 1 + 1 + 1; }
					else if (hasBuilding("courthouse")) { bonus_tax += 1 + 1; }
					else if (hasBuilding("temple")) { bonus_tax += 1; }

					// ── Land 계단식: armory → training_fields → barracks → regimental_camp → arsenal → conscription_center
					if (hasBuilding("conscription_center")) { bonus_man += 1 + 1 + 1 + 2 + 2 + 3; }
					else if (hasBuilding("arsenal")) { bonus_man += 1 + 1 + 1 + 2 + 2; }
					else if (hasBuilding("regimental_camp")) { bonus_man += 1 + 1 + 1 + 2; }
					else if (hasBuilding("barracks")) { bonus_man += 1 + 1 + 1; }
					else if (hasBuilding("training_fields")) { bonus_man += 1 + 1; }
					else if (hasBuilding("armory")) { bonus_man += 1; }

					// ── Naval 계단식: dock → drydock → shipyard → grand_shipyard → naval_arsenal → naval_base
					if (hasBuilding("naval_base")) { bonus_prod += 1 + 1 + 1 + 1 + 2 + 2; }
					else if (hasBuilding("naval_arsenal")) { bonus_prod += 1 + 1 + 1 + 1 + 2; }
					else if (hasBuilding("grand_shipyard")) { bonus_prod += 1 + 1 + 1 + 1; }
					else if (hasBuilding("shipyard")) { bonus_prod += 1 + 1 + 1; }
					else if (hasBuilding("drydock")) { bonus_prod += 1 + 1; }
					else if (hasBuilding("dock")) { bonus_prod += 1; }

					// ── Production 계단식: constable → workshop → counting_house → treasury_office → mint → stock_exchange
					if (hasBuilding("stock_exchange")) { bonus_tax += 1 + 1 + 0 + 1 + 1 + 2; bonus_prod += 0 + 0 + 1 + 0 + 0 + 0; }
					else if (hasBuilding("mint")) { bonus_tax += 1 + 1 + 0 + 1 + 1;   bonus_prod += 0 + 0 + 1 + 0 + 0; }
					else if (hasBuilding("treasury_office")) { bonus_tax += 1 + 1 + 0 + 1;     bonus_prod += 0 + 0 + 1 + 0; }
					else if (hasBuilding("counting_house")) { bonus_tax += 1 + 1 + 0;       bonus_prod += 0 + 0 + 1; }
					else if (hasBuilding("workshop")) { bonus_tax += 1 + 1; }
					else if (hasBuilding("constable")) { bonus_tax += 1; }

					// ── Trade 계단식: marketplace → trade_depot → canal → road_network → post_office → customs_house
					if (hasBuilding("customs_house")) { bonus_prod += 1 + 1 + 1 + 1 + 1 + 1; }
					else if (hasBuilding("post_office")) { bonus_prod += 1 + 1 + 1 + 1 + 1; }
					else if (hasBuilding("road_network")) { bonus_prod += 1 + 1 + 1 + 1; }
					else if (hasBuilding("canal")) { bonus_prod += 1 + 1 + 1; }
					else if (hasBuilding("trade_depot")) { bonus_prod += 1 + 1; }
					else if (hasBuilding("marketplace")) { bonus_prod += 1; }

					// ── Fort 계단식: fort1 → fort2 → fort3 → fort4 → fort5 → fort6
					if (hasBuilding("fort6")) { bonus_man += 1 + 1 + 1 + 1 + 2 + 2; }
					else if (hasBuilding("fort5")) { bonus_man += 1 + 1 + 1 + 1 + 2; }
					else if (hasBuilding("fort4")) { bonus_man += 1 + 1 + 1 + 1; }
					else if (hasBuilding("fort3")) { bonus_man += 1 + 1 + 1; }
					else if (hasBuilding("fort2")) { bonus_man += 1 + 1; }
					else if (hasBuilding("fort1")) { bonus_man += 1; }

					// ── Manufactories (독립 건물 - 계단식 아님)
					if (hasBuilding("refinery")) { bonus_prod += 2; }
					if (hasBuilding("wharf")) { bonus_prod += 2; }
					if (hasBuilding("weapons")) { bonus_man += 2; }
					if (hasBuilding("textile")) { bonus_prod += 2; }
					if (hasBuilding("fine_arts_academy")) { bonus_tax += 1; }
					if (hasBuilding("university")) { bonus_tax += 1; bonus_prod += 1; }

					// base_tax
					{
						int cur = 1;
						if (!eu4_province.GetItem("base_tax").empty())
							cur = std::stoi(eu4_province.GetItem("base_tax")[0].Get());
						int val = std::max(1, cur + bonus_tax);
						if (eu4_province.GetItem("base_tax").empty())
							eu4_province.AddItem("base_tax", std::to_string(val));
						else
							eu4_province.SetItem("base_tax", std::to_string(val));
					}
					// base_production
					{
						int cur = 1;
						if (!eu4_province.GetItem("base_production").empty())
							cur = std::stoi(eu4_province.GetItem("base_production")[0].Get());
						int val = std::max(1, cur + bonus_prod);
						if (eu4_province.GetItem("base_production").empty())
							eu4_province.AddItem("base_production", std::to_string(val));
						else
							eu4_province.SetItem("base_production", std::to_string(val));
					}
					// base_manpower
					{
						int cur = 1;
						if (!eu4_province.GetItem("base_manpower").empty())
							cur = std::stoi(eu4_province.GetItem("base_manpower")[0].Get());
						int val = std::max(1, cur + bonus_man);
						if (eu4_province.GetItem("base_manpower").empty())
							eu4_province.AddItem("base_manpower", std::to_string(val));
						else
							eu4_province.SetItem("base_manpower", std::to_string(val));
					}

					// ---- 건물 변환 끝 ----

					std::wstring _owner; std::string owner;
					if (!eu3_province->GetItemIdx("owner").empty()) {

						std::cout << Remove(eu3_province->GetItem("owner")[0].Get()) << "\n";
						owner = GetEU4Country(Remove(eu3_province->GetItem("owner")[0].Get()), eu3_vic_ct, y);
						for (int i = 0; i < owner.size(); ++i) {
							_owner.push_back(owner[i]);
						}
						std::cout << owner << " " << owner.empty() << " | ";
						if (owner.empty() == false) {
							if (eu4_province.GetItemIdx("owner").empty()) {
								eu4_province.AddItem("owner", "");
							}
							count++;
							chk = true;

							eu4_province.AddItem("add_core", owner);
							if (eu4_province.GetItem("is_city").empty()) {
								eu4_province.AddItem("is_city", "yes");
							}
							else {
								eu4_province.SetItem("is_city", "yes");
							}

							eu4_province.GetItemList(eu4_province.GetItemIdx("owner")[0]).Set(0, owner);

							eu4_province.AddUserTypeItem(clau_parser::UserType("1592.1.1"));

							eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("owner", owner);



							// ---- 테크 레벨 가중치 적용 (최대 72 기준) ----
							// owner의 테크 레벨을 가져와서 각 수치에 곱함
							{
								constexpr double MAX_TECH = 72.0;

								auto applyTechMul = [&](const std::string& itemName,
									std::map<std::wstring, int>& techMap, int max_tech) {
										auto it = techMap.find(_owner);
										if (it == techMap.end()) return;
										double mul = (double)it->second / max_tech;
										if (eu4_province.GetItem(itemName).empty()) return;
										int cur = std::stoi(eu4_province.GetItem(itemName)[0].Get());
										int val = std::max(1, (int)std::round(cur * mul * mul));
										eu4_province.SetItem(itemName, std::to_string(val));
									};

								applyTechMul("base_tax", eu4_countries_government_tech, max_gov);
								applyTechMul("base_manpower", eu4_countries_land_tech, max_land);

								// base_production은 production_tech, trade_tech, naval_tech 각각 곱한 평균
								{
									auto getTech = [&](std::map<std::wstring, int>& techMap) -> int {
										auto it = techMap.find(_owner);
										return (it != techMap.end()) ? it->second : 0;
										};

									if (!eu4_province.GetItem("base_production").empty()) {
										int cur = std::stoi(eu4_province.GetItem("base_production")[0].Get());
										double prod = getTech(eu4_countries_production_tech);
										double trade = getTech(eu4_countries_trade_tech);
										double naval = getTech(eu4_countries_naval_tech);
										double max_sum = (max_prod + max_trade + max_naval);
										double avg_mul = (prod + trade + naval) / max_sum;
										int val = std::max(1, (int)std::round(cur * avg_mul * avg_mul));
										eu4_province.SetItem("base_production", std::to_string(val));
									}
								}
							}
							// ---- 테크 가중치 끝 ----
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

					eu4_province.AddItem("discovered_by", "eastern");
					eu4_province.AddItem("discovered_by", "western");
					eu4_province.AddItem("discovered_by", "muslim");
					eu4_province.AddItem("discovered_by", "ottoman");

					eu4_province.SetName(std::to_string(std::stoi(eu4_province_name)));

					if (eu3_province->GetItem("religion").empty() == false) {
						std::string eu4_religion;
						std::string vic2_religion;
						std::string eu3_religion = eu3_province->GetItem("religion")[0].Get();
						auto& eu3 = eu3_vic2_religion.GetUserTypeList(0);

						bool pass = false;
						for (int i = 0; i < eu3->GetUserTypeListSize(); ++i) {
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

						if (_owner.empty() == false) {
							std::wcout << "pass " << pass << " owner " << _owner << "\n";
							eu4_religion = eu4_countries_primary_religion[_owner];
							if (eu4_religion == "") {
								std::cout << "empty ... \n";
								//exit(-1);
							}
							std::cout << "religion " << eu4_religion << "\n";
						}

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
								}
							}
							if (pass) {
								break;
							}
						}

						std::cout << eu4_culture << "\n";
						if (_owner.empty() == false) {
							std::wcout << "pass " << pass << " owner " << _owner << "\n";
							eu4_culture = eu4_countries_primary_culture[_owner];

							if (eu4_culture == "") {
								std::cout << "chk";
								//exit(-1);
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
				}

				eu3_eu4_provinces[eu3_province->GetName()] = eu4_province_name;
			}
			if (arr.empty()) {
				//
			}

		}
	}

	std::cout << "area " << area.GetIListSize() << "\n";
	for (int i = 0; i < area.GetUserTypeListSize(); ++i) {
		auto x = area.GetUserTypeList(i);

		std::map<std::string, std::tuple<int, std::string, std::string>> _map;

		for (int j = 0; j < x->GetItemListSize(); ++j) {
			auto y = x->GetItemList(j);

			auto& eu4_province = eu4_provinces[std::to_wstring(std::stoi(y.Get()))];

			if (eu4_province.GetItem("owner").empty()) {
				continue;
			}

			std::cout << " expand... owner [ " << j << " ] " << eu4_province.GetItem("owner")[0].Get() << "\n";
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

				if (_owner.empty() == false) {
					if (eu4_countries_primary_religion.find(_owner) != eu4_countries_primary_religion.end()) {
						religion = eu4_countries_primary_religion[_owner];
						std::cout << "chkddd";
					}
					std::cout << "religion " << religion << "\n";
				}
				if (_owner.empty() == false) {
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

		for (const auto& _ : _map) {
			if (val < std::get<0>(_.second)) {
				val = std::get<0>(_.second);
				winner = _.first;
				winner_religion = std::get<1>(_.second);
				winner_culture = std::get<2>(_.second);
			}
		}

		if (val != -1) {
			for (int j = 0; j < x->GetItemListSize(); ++j) {
				std::cout << "win " << winner << " " << winner_culture << "\n";

				auto y = x->GetItemList(j);

				auto& eu4_province = eu4_provinces[std::to_wstring(std::stoi(y.Get()))];

				if (eu4_province.GetItem("owner").empty()) {

					eu4_province.AddUserTypeItem(clau_parser::UserType("1592.1.1"));

					eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("owner", winner);
					eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("controller", winner);
					eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("religion", winner_religion);
					eu4_province.GetUserTypeList(eu4_province.GetUserTypeListSize() - 1)->AddItem("culture", winner_culture);

					eu4_province.AddItem("owner", winner);
					eu4_province.AddItem("add_core", winner);
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
					//exit(-1);
				}
				if (eu4_province.GetItem("culture").empty()) {
					eu4_province.AddItem("culture", winner_culture);
				}
				else {
					eu4_province.SetItem("culture", winner_culture);
				}

				// discovered_by는 여기서 추가하지 않음 (province 루프에서 이미 추가됨)
			}
		}
		else {
			std::cout << "chk --3" << "\n";
		}
	}

	std::cout << eu4_provinces.size() << "\n";

	for (auto& x : eu4_provinces) {
		auto eu4_province = x.second;
		auto eu4_province_name = x.first;

		clau_parser::LoadData::Save(eu4_province, std::wstring(L"output/history/provinces/") + eu4_province_name + L".txt");
	}

	{
		{
			std::cout << eu4_countries.size() << "\n";

			for (auto& x : eu4_countries) {
				auto eu4_country = x.second;
				auto eu4_country_name = x.first;

				clau_parser::LoadData::Save(eu4_country, std::wstring(L"output/history/countries/") + eu4_country_name + L".txt");
			}

			std::cout << "\n" << "count " << count << "\n";
		}
	}
	std::cout << "ends";
	return 0;
}
