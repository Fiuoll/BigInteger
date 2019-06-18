#include <iostream>
#include "biguint.h"
#include "moduint.h"
#include "primes.h"
#include "parser.h"
#include <CL/cl.h>
#include <time.h>
using namespace std;

#define DEBUG_PRINT 0
#define DEBUG_CHECK 0

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define CL_CATCH_ERROR( CODE, MSG ) \
   { if( (CODE) != CL_SUCCESS) { fprintf(stderr,"[E] " MSG ", line: %d, code: %s\n",__LINE__,get_error_string(CODE)); exit(-1); } }

#define STR_BUF_SIZE  1024


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char *kernel_src=
"__kernel void kernel1( __global unsigned long int *A, __global unsigned long int *B, __global int *P, __global unsigned long int *C, const int flag) {\n"
"    int idx = get_global_id(0); \n"
"    if (flag)\n"
"      C[idx] =  (A[idx] * B[idx]) % P[idx];\n "
"    else\n"
"      C[idx] = (A[idx] + B[idx]) % P[idx];\n"
"}";


const char *get_error_string(cl_int error);
int log_compiler_error(cl_program pro, cl_device_id dev);
int main(int argc, char **argv)
{
  vector<cl_platform_id> platforms; // список платформ
  cl_uint          num_platforms ;
  vector<cl_device_id> devices; // список вычислительных устройств
  cl_uint          num_devices;
  cl_context		context;
  cl_command_queue command_queue ; // очередь выполнения для выбранных устройств
  cl_program 		program; // программа для DEVICE
  cl_kernel 		kernel; // выполняемое ядро, созданное из программы
  cl_event		kernel_event;
  cl_int 		clStatus;

  double dt;

  char   queryBuffer[STR_BUF_SIZE];
  cl_uint queryInt;

  cl_uint wgDim;
  vector<size_t> wgSz;
  size_t wgSzMin;

  // оценка времени выполнения
  cl_ulong time_start, time_end;
  double total_time;

  // память под данные на DEVICE
  cl_mem clA;
  cl_mem clB;
  cl_mem clC;
  cl_mem clP;
  int cl_flag = 0;

  // память под данные на HOST

  ModUInt a, b, c;
  BigInt bia, bib, bic;

  size_t global_size; // всего процессов
  size_t local_size;  // количество процессов в группе

  int sel_platform_num = 0;
  int sel_device_num = 0;

//  switch(argc){
//         case 2: sel_platform_num=atoi(argv[1]); break;
//         case 3: sel_platform_num=atoi(argv[1]); sel_device_num=atoi(argv[2]); break;
//  }

  if (parse_command_line (argc, argv))
    return 1;

  // получить количество доступных платформ
  clStatus = clGetPlatformIDs(0, nullptr, &num_platforms);
  CL_CATCH_ERROR(clStatus,"clGetPlatformIDs");
  if (num_platforms < 1)
    {
      fprintf (stderr, "No OpenCL Platforms Found");
      return 1;
    }
  fprintf (stdout, "found %d platforms\n", num_platforms);

  if(!(sel_platform_num < num_platforms))
    {
      fprintf(stderr,"sel_platform_num out of range\n");
      return 1;
    }
  fprintf (stdout, "select platform #%d\n", sel_platform_num);

  // создаём буфер для списка платформ
  platforms.resize(num_platforms);
  // читаем список платформ
  clStatus = clGetPlatformIDs (num_platforms, platforms.data (), nullptr) ;
  CL_CATCH_ERROR(clStatus,"clGetPlatformIDs");

  // печатаем имя выбранной платформы
  clStatus = clGetPlatformInfo (platforms[sel_platform_num], CL_PLATFORM_NAME, STR_BUF_SIZE, queryBuffer, nullptr);
  CL_CATCH_ERROR(clStatus,"clGetPlatformInfo");
  fprintf (stdout,"CL_PLATFORM_NAME : %s\n", queryBuffer);

  // получить количество доступных устройств на платформе номер sel_platform_num
  clStatus= clGetDeviceIDs (platforms[sel_platform_num], CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices) ;
  CL_CATCH_ERROR(clStatus,"clGetDeviceIDs");
  if (num_devices < 1)
    {
      fprintf(stderr,"No OpenCL devices found for this platform");
      return 1;
    }
  fprintf(stdout, "found %d devices for this platform\n", num_devices);
  if(!(sel_device_num < num_devices))
    {
      fprintf(stderr,"sel_device_num out of range\n");
      return 1;
    }
  fprintf(stdout, "select device #%d\n", sel_device_num);

  // создаём буфер для списка устройств
  devices.resize(num_devices);

  // получить список доступных устройств на платформе номер sel_platform_num
  clStatus = clGetDeviceIDs (platforms[sel_platform_num], CL_DEVICE_TYPE_ALL, num_devices, devices.data(), &num_devices);
  CL_CATCH_ERROR(clStatus,"clGetDeviceIDs");

  // печатаем имя выбранного устройства
  clStatus = clGetDeviceInfo(devices[sel_device_num], CL_DEVICE_NAME, STR_BUF_SIZE, queryBuffer, nullptr);
  CL_CATCH_ERROR(clStatus,"clGetDeviceInfo");
  fprintf(stdout,"CL_DEVICE_NAME: %s\n", queryBuffer);

  // печатаем количество вычислительных модулей устройства
  clStatus = clGetDeviceInfo(devices[sel_device_num], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(queryInt), &queryInt, nullptr);
  CL_CATCH_ERROR(clStatus,"clGetDeviceInfo");
  fprintf(stdout,"CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", queryInt);

  // получаем максимальную размерность WORKGROUP
  clStatus = clGetDeviceInfo(devices[sel_device_num], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(wgDim), &wgDim, nullptr);
  CL_CATCH_ERROR(clStatus,"clGetDeviceInfo");
   fprintf(stdout,"CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %d\n", wgDim);

  // печатаем максимальные размеры WORKGROUP
  wgSz.resize(wgDim);
  clStatus = clGetDeviceInfo(devices[sel_device_num], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*wgDim, wgSz.data(), nullptr);
  fprintf(stdout,"CL_DEVICE_MAX_WORK_ITEM_SIZES: [");
  wgSzMin=wgSz[0]; // минимальный из размеров WORKGROUP
  for(unsigned int i = 0; i < wgDim; i++)
    {
      fprintf(stdout," %ld ",wgSz[i]);
      wgSzMin = (wgSzMin > wgSz[i]) ? wgSz[i] : wgSzMin;
    }
  fprintf(stdout,"]\n");

  // создаём контекст для устройства sel_device_num
  context = clCreateContext( nullptr, 1, &devices[sel_device_num], nullptr, nullptr, &clStatus);
  CL_CATCH_ERROR(clStatus,"error clCreateContext");

  // создать программу из текста ядра
  program = clCreateProgramWithSource(context, 1, &kernel_src, nullptr, &clStatus);
  CL_CATCH_ERROR(clStatus,"error clCreateProgramWithSource" );

  // компилируем программу-ядро
  clStatus = clBuildProgram(program, 1, &devices[sel_device_num], nullptr, nullptr, nullptr);
  if(clStatus != CL_SUCCESS)
    {
      log_compiler_error(program, devices[sel_device_num]);
      CL_CATCH_ERROR(clStatus,"error clBuildProgram" );
      return 8;
    }

  // создаём объект выполняемого ядра
  kernel = clCreateKernel(program, "kernel1", &clStatus);
  CL_CATCH_ERROR(clStatus,"error clCreateKernel" );

  // выделяем память device для данных
  clA = clCreateBuffer(context, CL_MEM_READ_ONLY, PRIME_BASE * sizeof(uint64_t), nullptr, &clStatus);
  CL_CATCH_ERROR(clStatus,"error clCreateBuffer" );
  clB = clCreateBuffer(context, CL_MEM_READ_ONLY, PRIME_BASE * sizeof(uint64_t), nullptr, &clStatus);
  CL_CATCH_ERROR(clStatus,"error clCreateBuffer" );
  clP = clCreateBuffer(context, CL_MEM_READ_ONLY, PRIME_BASE * sizeof(uint64_t), nullptr, &clStatus);
  CL_CATCH_ERROR(clStatus,"error clCreateBuffer" );
  clC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, PRIME_BASE * sizeof(uint64_t), nullptr, &clStatus);
  CL_CATCH_ERROR(clStatus,"error clCreateBuffer" );

  char buf1[1234];
  char buf2[1234];
  char op;

  BigUInt max_value = 1;
  for (auto &val : primes)
    max_value *= val;

  //check matrix
#if DEBUG_CHECK
  for (unsigned int i = 0; i < PRIME_BASE; i++)
    {
      for (unsigned int j = 0; j < PRIME_BASE; j++)
        {
          if (i == j)
            continue;
          BigUInt ch = primes[i];
          ch *= inverted_pi_mod_pj[i][j];
          if (ch % primes[j] != 1)
            {
              printf ("Wrong table!\n");
              return 1;
            }
        }
    }
#endif

  printf ("Input 'q' for quit\n");
  printf ("Values and result must be less then ");
  max_value.print();
  printf ("Number of degrees: %d\n", max_value.number_of_degrees());
  printf ("\n");
  printf ("Input [value] [+/-/*] [value]:\n");
  while (1)
    {
      scanf ("%s", buf1);

      if (buf1[0] == 'q')
        break;
      else if (buf1[0] == 'p')
        {
          print_p();
          continue;
        }
      else if (buf1[0] == 'r')
        {
          print_r();
          continue;
        }

      bia = buf1;
      a = static_cast<BigUInt> (buf1);
#if DEBUG_PRINT
      bia.print();
#endif
      scanf(" %c ", &op);
      scanf ("%s", buf2);

      bib = buf2;
      b = static_cast<BigUInt> (buf2);

      printf ("%s %c %s = ", buf1, op, buf2);
      dt = clock();

      switch (op)
        {
        case '+':
          cl_flag = 0;

          bic = bia + bib;
          break;
        case '-':
          bic = bia - bib;
          break;
        case '*':
          cl_flag = 1;
          bic = bia * bib;
          break;
        default:
          printf ("%c\n", op);
          break;
        }
      dt = (clock() - dt) ;
      bic.print();
      printf ("BigInt operation time: %.5f ms\n", dt * 10e-3);

#if DEBUG_PRINT
      BigUInt aaa = a.to_biguint();
      printf ("a = ");
      aaa.print();
#endif


      b = bib.to_biguint();
#if DEBUG_PRINT
      BigUInt bbb = bib.to_biguint();
      printf ("b = ");
      bbb.print();
#endif

#if 1
      dt = clock();
      switch (op)
        {
        case '+':
          c = a + b;
          break;
        case '-':
          c = a - b;
          break;
        case '*':
          c = a * b;
          break;
        default:
          printf ("%c\n", op);
          break;
        }
      dt = (clock() - dt) ;
      printf ("ModUInt operation time: %.5f ms\n", dt * 10e-3);

      BigUInt ccc = c.to_biguint();
      printf ("%s %c %s = ", buf1, op, buf2);
      ccc.print();
#endif

      // создаём очередь выполнения
      // command_queue = clCreateCommandQueueWithProperties(context, devices[sel_device_num], nullptr, &clStatus);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
      // для совместимости с nVidia SDK
      command_queue = clCreateCommandQueue(context, devices[sel_device_num], CL_QUEUE_PROFILING_ENABLE, &clStatus);
#pragma GCC diagnostic pop
      CL_CATCH_ERROR(clStatus,"error clCreateCommandQueue" );

      // копируем данные в память DEVICE
      clStatus = clEnqueueWriteBuffer(command_queue, clA, CL_TRUE, 0, PRIME_BASE * sizeof(uint64_t), a.data(), 0, nullptr, &kernel_event);
      CL_CATCH_ERROR(clStatus,"clEnqueueWriteBuffer");
      clStatus = clEnqueueWriteBuffer(command_queue, clB, CL_TRUE, 0, PRIME_BASE * sizeof(uint64_t), b.data(), 0, nullptr, &kernel_event);
      CL_CATCH_ERROR(clStatus,"clEnqueueWriteBuffer");
      clStatus= clEnqueueWriteBuffer(command_queue, clP, CL_TRUE, 0, PRIME_BASE * sizeof(int), primes.data(), 0, nullptr, &kernel_event);
      CL_CATCH_ERROR(clStatus,"clEnqueueWriteBuffer");

      // назначаем параметры ядра
      clStatus = clSetKernelArg(kernel, 0, sizeof(cl_mem), &clA);
      clStatus |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &clB);
      clStatus |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &clP);
      clStatus |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &clC);
      clStatus |= clSetKernelArg(kernel, 4, sizeof(int), &cl_flag);
      CL_CATCH_ERROR(clStatus,"error clSetKernelArg");

      // количество процессов
      global_size = PRIME_BASE; // всего процессов WORKITEM
      local_size = wgSzMin > global_size ? global_size : wgSzMin;      // количество процессов в одной WORKGROUP
      fprintf(stdout,"run kernel, work items: %ld, workgroup size: %ld\n", global_size, local_size);

      // запускаем ядро
      clStatus = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr, &global_size, nullptr, 0, nullptr, &kernel_event);
      CL_CATCH_ERROR(clStatus,"clEnqueueNDRangeKernel");

      // считываем результат
      clStatus = clEnqueueReadBuffer(command_queue, clC, CL_TRUE, 0, PRIME_BASE * sizeof(uint64_t), c.data(), 1, &kernel_event, nullptr);
      CL_CATCH_ERROR(clStatus,"clEnqueueReadBuffer");

      // ждём завершения всех операций
      clStatus = clFinish(command_queue);
      CL_CATCH_ERROR( clStatus,"clFinish") ;

      // печатаем результат
#if DEBUG_CHECK
      int er = 0;
      for (unsigned int i = 0; i < PRIME_BASE; i++)
        {
#if DEBUG_PRINT
          printf ("%d == %d\n", (a[i] + b[i]) % primes[i], c[i]);
#endif
          if(    (cl_flag && (a[i] * b[i]) % primes[i] != c[i])
              || (!cl_flag && (a[i] + b[i]) % primes[i] != c[i]))
            {
              fprintf(stdout,"error [%i]\t\t. %.1d\t+ %.1d\t %.1d\t != \t%.1d\n", i, a[i], b[i], primes[i], c[i]);
              er = 1;
              break;
            }
        }
      if(!er)
        fprintf(stdout,"\nsuccessfuly finished\n");
#endif
      printf ("%s %c %s = ", buf1, op, buf2);
      dt = clock();
      bic = c.to_bigint();
      dt = (clock() - dt) ;
      bic.print();
      printf ("Converting from ModUInt to BigUInt time: %.5f ms\n", dt * 10e-3);

      // считаем затраченное время
      clStatus = clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, nullptr);
      CL_CATCH_ERROR( clStatus,"clGetEventProfilingInfo") ;
      clStatus = clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, nullptr);
      CL_CATCH_ERROR( clStatus,"clGetEventProfilingInfo") ;
      total_time = time_end - time_start;
      fprintf(stdout,"\nexecution time: %.5f ms\n", (total_time * 10e-6f) );

      clReleaseCommandQueue(command_queue);
    }

  // освобождаем ресурсы
  clReleaseMemObject(clA);
  clReleaseMemObject(clB);
  clReleaseMemObject(clC);

  clReleaseKernel(kernel);
  clReleaseProgram(program);

  clReleaseContext(context);

  return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// печатает информацию об ошибках компиляции ядра
int log_compiler_error(cl_program pro, cl_device_id dev) {
   vector<char> buildLog;
   size_t buildLogSize = 0;
   cl_int 			clStatus;

   clStatus= clGetProgramBuildInfo(pro, dev, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog.data(), &buildLogSize);
   CL_CATCH_ERROR(clStatus,"clGetProgramBuildInfo");

   buildLog.resize (buildLogSize);
   bzero(buildLog.data(), sizeof(char)*buildLogSize);

   clStatus= clGetProgramBuildInfo(pro, dev, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog.data(), nullptr);
   CL_CATCH_ERROR(clStatus,"clGetProgramBuildInfo");

   fprintf(stderr,"KERNEL BUILD LOG:\n%s\n",buildLog.data());

   return 0;;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// расшифровка кода ошибки
const char *get_error_string(cl_int code) {
   switch(code){
          case CL_SUCCESS: return "CL_SUCCESS";
          // run-time and JIT compiler errors
          case CL_DEVICE_NOT_FOUND: return "CL_DEVICE_NOT_FOUND";
          case CL_DEVICE_NOT_AVAILABLE: return "CL_DEVICE_NOT_AVAILABLE";
          case CL_COMPILER_NOT_AVAILABLE: return "CL_COMPILER_NOT_AVAILABLE";
          case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
          case CL_OUT_OF_RESOURCES: return "CL_OUT_OF_RESOURCES";
          case CL_OUT_OF_HOST_MEMORY: return "CL_OUT_OF_HOST_MEMORY";
          case CL_PROFILING_INFO_NOT_AVAILABLE: return "CL_PROFILING_INFO_NOT_AVAILABLE";
          case CL_MEM_COPY_OVERLAP: return "CL_MEM_COPY_OVERLAP";
          case CL_IMAGE_FORMAT_MISMATCH: return "CL_IMAGE_FORMAT_MISMATCH";
          case CL_IMAGE_FORMAT_NOT_SUPPORTED: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
          case CL_BUILD_PROGRAM_FAILURE: return "CL_BUILD_PROGRAM_FAILURE";
          case CL_MAP_FAILURE: return "CL_MAP_FAILURE";
          case CL_MISALIGNED_SUB_BUFFER_OFFSET: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
          case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
          case CL_COMPILE_PROGRAM_FAILURE: return "CL_COMPILE_PROGRAM_FAILURE";
          case CL_LINKER_NOT_AVAILABLE: return "CL_LINKER_NOT_AVAILABLE";
          case CL_LINK_PROGRAM_FAILURE: return "CL_LINK_PROGRAM_FAILURE";
          case CL_DEVICE_PARTITION_FAILED: return "CL_DEVICE_PARTITION_FAILED";
          case CL_KERNEL_ARG_INFO_NOT_AVAILABLE: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

	  // compile-time errors
	  case CL_INVALID_VALUE: return "CL_INVALID_VALUE";
	  case CL_INVALID_DEVICE_TYPE: return "CL_INVALID_DEVICE_TYPE";
	  case CL_INVALID_PLATFORM: return "CL_INVALID_PLATFORM";
	  case CL_INVALID_DEVICE: return "CL_INVALID_DEVICE";
	  case CL_INVALID_CONTEXT: return "CL_INVALID_CONTEXT";
	  case CL_INVALID_QUEUE_PROPERTIES: return "CL_INVALID_QUEUE_PROPERTIES";
	  case CL_INVALID_COMMAND_QUEUE: return "CL_INVALID_COMMAND_QUEUE";
	  case CL_INVALID_HOST_PTR: return "CL_INVALID_HOST_PTR";
	  case CL_INVALID_MEM_OBJECT: return "CL_INVALID_MEM_OBJECT";
	  case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	  case CL_INVALID_IMAGE_SIZE: return "CL_INVALID_IMAGE_SIZE";
	  case CL_INVALID_SAMPLER: return "CL_INVALID_SAMPLER";
	  case CL_INVALID_BINARY: return "CL_INVALID_BINARY";
	  case CL_INVALID_BUILD_OPTIONS: return "CL_INVALID_BUILD_OPTIONS";
	  case CL_INVALID_PROGRAM: return "CL_INVALID_PROGRAM";
	  case CL_INVALID_PROGRAM_EXECUTABLE: return "CL_INVALID_PROGRAM_EXECUTABLE";
	  case CL_INVALID_KERNEL_NAME: return "CL_INVALID_KERNEL_NAME";
	  case CL_INVALID_KERNEL_DEFINITION: return "CL_INVALID_KERNEL_DEFINITION";
	  case CL_INVALID_KERNEL: return "CL_INVALID_KERNEL";
	  case CL_INVALID_ARG_INDEX: return "CL_INVALID_ARG_INDEX";
	  case CL_INVALID_ARG_VALUE: return "CL_INVALID_ARG_VALUE";
	  case CL_INVALID_ARG_SIZE: return "CL_INVALID_ARG_SIZE";
	  case CL_INVALID_KERNEL_ARGS: return "CL_INVALID_KERNEL_ARGS";
	  case CL_INVALID_WORK_DIMENSION: return "CL_INVALID_WORK_DIMENSION";
	  case CL_INVALID_WORK_GROUP_SIZE: return "CL_INVALID_WORK_GROUP_SIZE";
	  case CL_INVALID_WORK_ITEM_SIZE: return "CL_INVALID_WORK_ITEM_SIZE";
	  case CL_INVALID_GLOBAL_OFFSET: return "CL_INVALID_GLOBAL_OFFSET";
	  case CL_INVALID_EVENT_WAIT_LIST: return "CL_INVALID_EVENT_WAIT_LIST";
	  case CL_INVALID_EVENT: return "CL_INVALID_EVENT";
	  case CL_INVALID_OPERATION: return "CL_INVALID_OPERATION";
	  case CL_INVALID_GL_OBJECT: return "CL_INVALID_GL_OBJECT";
	  case CL_INVALID_BUFFER_SIZE: return "CL_INVALID_BUFFER_SIZE";
	  case CL_INVALID_MIP_LEVEL: return "CL_INVALID_MIP_LEVEL";
	  case CL_INVALID_GLOBAL_WORK_SIZE: return "CL_INVALID_GLOBAL_WORK_SIZE";
	  case CL_INVALID_PROPERTY: return "CL_INVALID_PROPERTY";
	  case CL_INVALID_IMAGE_DESCRIPTOR: return "CL_INVALID_IMAGE_DESCRIPTOR";
	  case CL_INVALID_COMPILER_OPTIONS: return "CL_INVALID_COMPILER_OPTIONS";
	  case CL_INVALID_LINKER_OPTIONS: return "CL_INVALID_LINKER_OPTIONS";
	  case CL_INVALID_DEVICE_PARTITION_COUNT: return "CL_INVALID_DEVICE_PARTITION_COUNT";

	  /* extension errors
	  case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
	  case CL_PLATFORM_NOT_FOUND_KHR: return "CL_PLATFORM_NOT_FOUND_KHR";
	  case CL_INVALID_D3D10_DEVICE_KHR: return "CL_INVALID_D3D10_DEVICE_KHR";
	  case CL_INVALID_D3D10_RESOURCE_KHR: return "CL_INVALID_D3D10_RESOURCE_KHR";
	  case CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
	  case CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
	  */
   }
   return "Unknown OpenCL error code";
}


