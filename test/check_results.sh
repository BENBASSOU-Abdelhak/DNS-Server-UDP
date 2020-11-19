#!/bin/bash


# Tests sur les compilations

if  gcc ../Root_DNS.c -o Root_DNS; then
    printf "\033[32mSUCCESS_Root_Compilation\033[0m\n"


else
    printf "\n\033[31mFAIL_Root_Compilation\033[0m\n"

fi


if  gcc ../Local_DNS.c -o Local_DNS; then
    printf "\033[32mSUCCESS_Local_Compilation\033[0m\n"

else
    printf "\n\033[31mFAIL_Local_Compilation\033[0m\n"

fi


if  gcc ../TLD_DNS.c -o TLD_DNS; then
    printf "\033[32mSUCCESS_TLD_Compilation\033[0m\n"

else
    printf "\n\033[31mFAIL_TLD_Compilation\033[0m\n"

fi

if  gcc ../TLD_in.c -o TLD_in; then
    printf "\033[32mSUCCESS_TLD_Compilation\033[0m\n"

else
    printf "\n\033[31mFAIL_TLD_Compilation\033[0m\n"

fi

if  gcc ../Authoritative_DNS.c -o Authoritative_DNS; then
    printf "\033[32mSUCCESS_Authoritative_DNS_Compilation\033[0m\n"

else
    printf "\n\033[31mFAIL_Authoritative_DNS_Compilation\033[0m\n"

fi

if  gcc ../Authoritative_in.c -o Authoritative_in; then
    printf "\033[32mSUCCESS_Authoratative_in_Compilation\033[0m\n"

else
    printf "\n\033[31mFAIL_Authoratative_in_Compilation\033[0m\n"

fi

if  gcc ../Client.c -o Client; then
    printf "\033[32mSUCCESS_Client_Compilation\033[0m\n"

else
    printf "\n\033[31mFAIL_Client_Compilation\033[0m\n"

fi
rm Client Local_DNS Root_DNS TLD_DNS TLD_in Authoritative_DNS Authoritative_in



#Test : les résultats renvoyés sont correctes?

if diff  ../Fichiers_txt/localcache.txt ../Fichiers_txt/domain_correction.txt
 then
  printf "\033[32mResult_SUCCESS\033[0m\n"
    exit 0
  else
    printf "\n\033[31mResult_FAIL\033[0m\n"
    exit 1
  fi
