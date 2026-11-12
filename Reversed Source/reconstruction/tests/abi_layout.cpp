// Compile with a 32-bit GNU C++ ABI, e.g. g++ -m32 -std=c++03 -fsyntax-only.
// Compile-time checks do not require linking or running the target server.
#include <cstddef>
#include "../../game/src/FSM.h"
#include "../../game/src/event.h"
#include "../../game/src/event_queue.h"
#include "../../game/src/vector.h"
#include "../../game/src/file_loader.h"
#include "../../game/src/crc32.h"
#include "../../libthecore/include/heart.h"
#include "../../libthecore/include/buffer.h"
#include "../../game/src/buffer_manager.h"

#define CHECK(name, expression) typedef char name[(expression) ? 1 : -1]
CHECK(pointer_is_32_bits, sizeof(void*) == 4);
CHECK(long_is_32_bits, sizeof(long) == 4);
CHECK(dword_is_32_bits, sizeof(DWORD) == 4);
CHECK(state_size, sizeof(CState) == 4);
CHECK(state_template_size, sizeof(CStateTemplate<CFSM>) == 32);
CHECK(fsm_size, sizeof(CFSM) == 44);
CHECK(queue_element_size, sizeof(TQueueElement) == 12);
CHECK(queue_size, sizeof(CEventQueue) == 16);
CHECK(queue_data_offset, offsetof(TQueueElement, pvData) == 0);
CHECK(queue_key_offset, offsetof(TQueueElement, iKey) == 4);
CHECK(queue_cancel_offset, offsetof(TQueueElement, bCancel) == 8);
CHECK(info_size, sizeof(EVENTINFO) == 8);
CHECK(info_id_offset, offsetof(EVENTINFO, dwID) == 0);
CHECK(info_data_offset, offsetof(EVENTINFO, pvData) == 4);
CHECK(event_size, sizeof(EVENT) == 88);
CHECK(event_func_offset, offsetof(EVENT, func) == 0);
CHECK(event_id_offset, offsetof(EVENT, dwInfoID) == 4);
CHECK(event_info_offset, offsetof(EVENT, info) == 8);
CHECK(event_queue_offset, offsetof(EVENT, q_el) == 12);
CHECK(event_force_offset, offsetof(EVENT, is_force_to_end) == 16);
CHECK(event_processing_offset, offsetof(EVENT, is_processing) == 17);
CHECK(event_file_offset, offsetof(EVENT, file) == 18);
CHECK(event_line_offset, offsetof(EVENT, line) == 84);
CHECK(heart_size, sizeof(HEART) == 36);
CHECK(heart_func_offset, offsetof(HEART, func) == 0);
CHECK(heart_before_offset, offsetof(HEART, before_sleep) == 4);
CHECK(heart_opt_offset, offsetof(HEART, opt_time) == 12);
CHECK(heart_last_offset, offsetof(HEART, last_time) == 20);
CHECK(heart_rate_offset, offsetof(HEART, passes_per_sec) == 28);
CHECK(heart_pulse_offset, offsetof(HEART, pulse) == 32);
CHECK(vector_size, sizeof(VECTOR) == 12);
CHECK(vector_x_offset, offsetof(VECTOR, x) == 0);
CHECK(vector_y_offset, offsetof(VECTOR, y) == 4);
CHECK(vector_z_offset, offsetof(VECTOR, z) == 8);
CHECK(memory_loader_size, sizeof(CMemoryTextFileLoader) == 16);
CHECK(crc_type_size, sizeof(crc_t) == 4);
CHECK(buffer_size_abi, sizeof(BUFFER) == 32);
CHECK(buffer_next_offset, offsetof(BUFFER, next) == 0);
CHECK(buffer_write_offset, offsetof(BUFFER, write_point) == 4);
CHECK(buffer_write_pos_offset, offsetof(BUFFER, write_point_pos) == 8);
CHECK(buffer_read_offset, offsetof(BUFFER, read_point) == 12);
CHECK(buffer_length_offset, offsetof(BUFFER, length) == 16);
CHECK(buffer_data_offset, offsetof(BUFFER, mem_data) == 20);
CHECK(buffer_mem_size_offset, offsetof(BUFFER, mem_size) == 24);
CHECK(buffer_flag_offset, offsetof(BUFFER, flag) == 28);
CHECK(buffer_manager_size, sizeof(BUFFER_MANAGER) == 44);
CHECK(buffer_singleton_size, sizeof(singleton<BUFFER_MANAGER>) == 4);
CHECK(temp_buffer_size, sizeof(TEMP_BUFFER) == 4);
#undef CHECK
