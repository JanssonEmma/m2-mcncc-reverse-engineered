// Insert into common/tables.h with the original packed records.
// Old DWARF: time_t -> __time_t -> __int32_t -> signed int (4 bytes).
// Store that exact underlying scalar explicitly; host CRT time_t may be 8 bytes.
// Do not redefine the CRT's global time_t or include time.h for this record.
#pragma pack(push, 1)
typedef struct SPlayerSkill {
    BYTE bMasterType;
    BYTE bLevel;
    int tNextRead; // original time_t, target signed int32, offset2
} TPlayerSkill;
#pragma pack(pop)
