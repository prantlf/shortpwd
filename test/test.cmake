file(WRITE actual.txt "\$ ${EXE} 0 9\n")
execute_process(COMMAND "${BIN}/${EXE}" 0 9
  OUTPUT_VARIABLE OUTPUT)
file(APPEND actual.txt "${OUTPUT}")

file(APPEND actual.txt "\$ ${EXE} 0 9 ''\n")
execute_process(COMMAND "${BIN}/${EXE}" 0 9 ""
  OUTPUT_VARIABLE OUTPUT)
file(APPEND actual.txt "${OUTPUT}")

file(APPEND actual.txt "\$ ${EXE} 0 9 ..\n")
execute_process(COMMAND "${BIN}/${EXE}" 0 9 ..
  OUTPUT_VARIABLE OUTPUT)
file(APPEND actual.txt "${OUTPUT}")

execute_process(COMMAND diff -u "${SRC}/test/expected.txt" "${BIN}/actual.txt"
  OUTPUT_VARIABLE OUTPUT
  RESULT_VARIABLE RESULT)
if(RESULT EQUAL 0)
  message(STATUS "The actual test output matches the expected ooe.")
elseif(RESULT EQUAL 1)
  message("${OUTPUT}")
  message(SEND_ERROR "The actual test output differs from the expected one.")
else()
  message(SEND_ERROR "Comparing the actual and expected test output failed.")
endif()
