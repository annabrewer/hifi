//
//  Created by Amer Cerkic 05/02/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "MACOSPlatform.h"
#include "../PlatformKeys.h"

#include <thread>
#include <string>
#include <CPUIdent.h>
#include <GPUIdent.h>

#ifdef Q_OS_MAC
#include <unistd.h>
#include <cpuid.h>
#include <sys/sysctl.h>
#endif

using namespace platform;

void MACOSInstance::enumerateCpu() {
    json cpu = {};

    cpu[keys::cpu::vendor] = CPUIdent::Vendor();
    cpu[keys::cpu::model] = CPUIdent::Brand();
    cpu[keys::cpu::numCores] = std::thread::hardware_concurrency();

    _cpu.push_back(cpu);
}

void MACOSInstance::enumerateGpu() {
    GPUIdent* ident = GPUIdent::getInstance();
    json gpu = {};
    gpu[keys::gpu::vendor] = ident->getName().toUtf8().constData();
    gpu[keys::gpu::model] = ident->getName().toUtf8().constData();
    gpu[keys::gpu::videoMemory] = ident->getMemory();
    gpu[keys::gpu::driver] = ident->getDriver().toUtf8().constData();

    _gpu.push_back(gpu);
    _display = ident->getOutput();

}

void MACOSInstance::enumerateMemory() {
    json ram = {};

#ifdef Q_OS_MAC
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    ram[keys::memTotal] = pages * page_size;
#endif
    _memory.push_back(ram);
}

void MACOSInstance::enumerateComputer(){
    _computer[keys::computer::OS] = keys::computer::OS_MACOS;
    _computer[keys::computer::vendor] = keys::computer::vendor_Apple;

#ifdef Q_OS_MAC

    //get system name
    size_t len=0;
    sysctlbyname("hw.model",NULL, &len, NULL, 0);
    char* model = (char *) malloc(sizeof(char)*len+1);
    sysctlbyname("hw.model", model, &len, NULL,0);
    
    _computer[keys::computer::model]=std::string(model);

    free(model);
    
#endif
}

