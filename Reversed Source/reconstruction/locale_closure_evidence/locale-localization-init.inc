// Complete original locale_service.cpp1017, 080686c0.
bool LC_InitLocalization(const std::string& szLocal)
{
    g_stLocal=szLocal;
    if(g_stLocal=="ymir") g_eLocalType=LC_YMIR;
    else if(g_stLocal=="letsgame") g_eLocalType=LC_LETSGAME;
    else if(g_stLocal=="cibn") g_eLocalType=LC_CIBN;
    else if(g_stLocal=="japan") g_eLocalType=LC_JAPAN;
    else if(g_stLocal=="english") g_eLocalType=LC_ENGLISH;
    else if(g_stLocal=="hongkong") g_eLocalType=LC_HONGKONG;
    else if(g_stLocal=="newcibn") g_eLocalType=LC_NEWCIBN;
    else if(g_stLocal=="germany") g_eLocalType=LC_GERMANY;
    else if(g_stLocal=="korea") g_eLocalType=LC_KOREA;
    else if(g_stLocal=="france") g_eLocalType=LC_FRANCE;
    else if(g_stLocal=="italy") g_eLocalType=LC_ITALY;
    else if(g_stLocal=="spain") g_eLocalType=LC_SPAIN;
    else if(g_stLocal=="uk") g_eLocalType=LC_UK;
    else if(g_stLocal=="turkey") g_eLocalType=LC_TURKEY;
    else if(g_stLocal=="poland") g_eLocalType=LC_POLAND;
    else if(g_stLocal=="portugal") g_eLocalType=LC_PORTUGAL;
    else if(g_stLocal=="canada") g_eLocalType=LC_CANADA;
    else if(g_stLocal=="brazil") g_eLocalType=LC_BRAZIL;
    else return false;
    return true;
}
