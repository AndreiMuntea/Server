#ifndef UTILS_DEFINITIONS_H
#define UTILS_DEFINITIONS_H

typedef long STATUS;

#define ERROR_BITS                  0xFFFF0000
#define WARNING_BITS                0x0000FFFF
#define INTERNAL_ERROR_BITS         0xF0000000

#define EXIT_SUCCESS_STATUS         0x00000000
#define SUCCESS(status)             (!((status) & ERROR_BITS))
#define INTERNAL_ERROR(status)      ((status) & INTERNAL_ERROR_BITS)

#define BAD_ALLOCATION              0x10000000
#define NULL_POINTER                0x20000000
#define INVALID_PARAMETER           0x30000000
#define FILE_IO_ERROR               0x40000000
#define FILE_OPEN_ERROR             0x50000000
#define INSUFICIENT_BUFFER          0x60000000
#define CRITICAL_SECTION_FAILURE    0x70000000
#define PIPE_FAILURE                0x80000000
            
#endif //UTILS_DEFINITIONS_H