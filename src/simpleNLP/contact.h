/*
 * contact.h
 *
 *  Created on: May 13, 2015
 *      Author: litton
 */

#ifndef CONTACT_H_
#define CONTACT_H_

#include<iostream>
#include<string>
#include<tr1/regex>

using namespace std;

bool isPhoneNumber(string s){
	std::tr1::regex acc ("((\\+?\\d{2,3}-?)|(\\(\\d{2,4}\\)))?(\\d{3,8}-?){2,4}\\d");//( "(\\+?\\d{2,3}-?|\\(\\d{2,4}\\))?(\\d{3,8}-?){2,4}\d" );
	std::tr1::regex rej("(\\S*\\D\\d)|(.*-\\d?-.*)");

	bool b1 = regex_match(s,acc);
	bool b2 = regex_match(s,rej);
	cout<<b1<<" "<<b2<<endl;
	if(b1&&!b2)
		return true;
	return false;
}

bool isEmail(string s){
	std::tr1::regex acc ("[\\w|-]+@(\\w{1,10}\\.){1,2}[A-Za-z]{2,3}");//( "[\\W|-]+@[\\W]{1,10}\\.){1,2}[A-Za-z]{2,3}" );
	std::tr1::regex rej (".*((-@)|(@-))|(-\\.).*");//("(.*((-@)|(-\\.)|(@-)).*");

	bool b1 = regex_match(s,acc);
	bool b2 = regex_match(s,rej);
	cout<<b1<<" "<<b2<<endl;
	if(b1&&!b2)
		return true;
	return false;
}

#endif /* CONTACT_H_ */
