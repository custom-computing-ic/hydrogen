#include <message.hpp>

msg_t* msg_empty() {
  msg_t *msg = (msg_t *)calloc(sizeof(msg_t), 1);
  return msg;
}
