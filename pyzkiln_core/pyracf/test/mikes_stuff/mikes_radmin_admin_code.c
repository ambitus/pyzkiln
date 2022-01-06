#include <unistd.h>
#include <signal.h>
#include <stdio.h>
size_t read_stdin(char* buffer, size_t max) {
	size_t bytes;
	size_t buffer_remaining = max;
	size_t bytes_read = 0;
	do {
		bytes = read(STDIN_FILENO, &buffer[bytes_read], buffer_remaining);
		bytes_read += bytes;
		if (bytes_read == max) {
		  fputs("Input buffer too small to read entire file. Break file into smaller chunks\n", stderr);
			return 0;
		}
		if (bytes_read < 0) {
			fputs("Unexpected error occurred reading from stdin. Processing stopped\n", stderr);
			return 0;
		}
		buffer_remaining -= bytes_read;
	} while (bytes > 0);
	return bytes_read;
}
#define ONE_HUNDRED_MEGABYTES (100000000)
typedef struct {
	unsigned char len;
	char str[8];
} VarStr_T;
typedef struct {
	char* work_area;
	void** alet1;
	unsigned int* saf_rc;
	void** alet2;
	unsigned int* racf_rc;
	void** alet3;
	unsigned int* racf_rsn;
	unsigned int* num_parms;
	unsigned int* fn;
	unsigned int* opts;
	unsigned int* req_len;
	const char* req;
	char* req_handle;
	VarStr_T* userid;
	void** acee;
	unsigned int* rsp_len;
	char* rsp;
} SecMgr_T;
unsigned int IRRSMO00(SecMgr_T sec_mgr);
#pragma map(IRRSMO00, "IRRSMO00")
int main() {
	char work_area[1024];
	char req_handle[64] = { 0 };
	VarStr_T userid = { 0, {0}};
	void* alet = NULL;
	void* acee = NULL;
	char buffer[ONE_HUNDRED_MEGABYTES+1];
	size_t bytes;
	char rsp[ONE_HUNDRED_MEGABYTES+1];
	SecMgr_T sec_mgr = { 0 };
	void* fp;
	unsigned int saf_rc, racf_rc, racf_rsn;
	unsigned int num_parms=17, fn=1, opts=1, rsp_len = sizeof(rsp)-1;
	bytes  = read_stdin(buffer, sizeof(buffer)-1);
	buffer[bytes] = '\0';
	sec_mgr.work_area = work_area;
	sec_mgr.alet1 = &alet;
	sec_mgr.saf_rc = &saf_rc;
	sec_mgr.alet2 = &alet;
	sec_mgr.racf_rc = &racf_rc;
	sec_mgr.alet3 = &alet;
	sec_mgr.racf_rsn = &racf_rsn;
	sec_mgr.num_parms = &num_parms;
	sec_mgr.fn = &fn;
	sec_mgr.opts = &opts;
	sec_mgr.req_len = &bytes;
	sec_mgr.req = buffer;
	sec_mgr.req_handle = req_handle;
	sec_mgr.userid = &userid;
	sec_mgr.acee = &acee;
	sec_mgr.rsp = rsp;
	sec_mgr.rsp_len = &rsp_len;
	IRRSMO00(sec_mgr);
	if (saf_rc > 8) {
		fprintf(stderr, "Call to security manager failed. SAF_RC 0x%x RACF_RC 0x%x RACF_RSN 0x%x\n", saf_rc, racf_rc, racf_rsn);
		fprintf(stderr, "Response %p Response Length: %d\n", rsp, rsp_len);
		return saf_rc;
	} else {
		if (saf_rc == 4) {
			fprintf(stderr, "Input XML Processed but warnings occurred. SAF_RC 0x%x RACF_RC 0x%x RACF_RSN 0x%x\n", saf_rc, racf_rc, racf_rsn);
		}
		write(STDOUT_FILENO, rsp, rsp_len);
	}
	return 0;
}