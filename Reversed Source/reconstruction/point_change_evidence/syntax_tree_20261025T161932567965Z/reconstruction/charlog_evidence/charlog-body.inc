
//080785a0: exact old six-argument CHARACTER audit path.
void LogManager::CharLog(DWORD pid,DWORD x,DWORD y,DWORD value,const char* text,const char* hint) {
    EscapeString(__escape_hint,hint,std::strlen(hint));
    const char* format="INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), %lu, %lu, %lu, %lu, '%s', '%s')";
    sys_log(0,format,get_table_postfix(),pid,x,y,value,text,hint);
    Query(format,get_table_postfix(),pid,x,y,value,text,__escape_hint);
}
