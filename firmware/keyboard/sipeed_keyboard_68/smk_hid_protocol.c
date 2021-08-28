//
// Created by hailin on 2021/8/26.
//

#include "smk_hid_protocol.h"
#include <string.h>
static hid_data_reg_t *map_list[MAX_PROTOCOL_MAP_ENTRIES];
static hid_data_reg_t **map_list_end=&map_list[MAX_PROTOCOL_MAP_ENTRIES];
static inline int min(int a,int b)
{
    return a<b?a:b;
}
void hid_data_protocol_init()
{
    memset(map_list,0,sizeof(map_list));
}
int hid_data_protocal_reg(hid_data_reg_t* pro)
{
    int i;
    for(i=0;i<MAX_PROTOCOL_MAP_ENTRIES;i++){
        if(map_list[i]==NULL)
            break;
    }
    if(i==MAX_PROTOCOL_MAP_ENTRIES)
        return -1;
    map_list[i]=pro;
    return 0;
}
int hid_data_protocal_callback(hid_data_report_t *report,hid_data_reg_t *out)
{
    static hid_data_report_t ret_report;
    ret_report.reportID = report->reportID;
    ret_report.packageID = report->packageID + 1;
    if (report->reportID == 1||report->reportID == 2) { //READ
        if (report->datalen > MAX_PAK_DATALEN)
            return -1;
        uint8_t totalread = report->datalen;
        hid_data_reg_t **map = map_list;
        ret_report.datalen = 0;
        while (*map && map != map_list_end) {
            if (report->addr >= (*map)->base)
                if ((*map)->datatype == data_type_fixed) {
                    if (report->addr < (*map)->base + (*map)->size) {
                        uint32_t datatoread = min(((*map)->size + (*map)->base - report->addr), report->datalen);
                        uint32_t datareaded = 0;
                        if ((*map)->maptype == map_type_data) {
                            if(report->reportID==1)
                                memcpy(ret_report.data + ret_report.datalen, (*map)->data + (report->addr - (*map)->base), datatoread);
                            else
                                memcpy( (*map)->data + (report->addr - (*map)->base),report->data + ret_report.datalen, datatoread);
                            datareaded = datatoread;
                        } else if ((*map)->maptype == map_type_func) {
                            int ret;
                            if(report->reportID==1)
                                ret=(*map)->read((report->addr - (*map)->base), ret_report.data + ret_report.datalen, datatoread);
                            else
                                ret=(*map)->write((report->addr - (*map)->base), report->data + ret_report.datalen, datatoread);

                            if (ret < 0) {
                                ret_report.datalen = 0xff;
                                break;
                            }
                            datareaded = ret;
                        }
                        ret_report.datalen += datareaded;
                        if (ret_report.datalen == totalread)
                            break;
                        if (datatoread == report->datalen)
                            break;
                        ret_report.datalen -= datareaded;
                        ret_report.datalen += datatoread;
                        report->addr += datatoread;
                        report->datalen -= datatoread;
                        //continue;
                    }
                } else {
                    if (report->addr == (*map)->base) {
                        int (*func)(uint32_t addr,uint8_t* data,uint8_t len);
                        int ret;
                        if(report->reportID==1)
                            ret=(*map)->read(0, ret_report.data + ret_report.datalen, report->datalen);
                        else
                            ret=(*map)->write(0, report->data + ret_report.datalen, report->datalen);
                        if (ret < 0) {
                            ret_report.datalen = 0xff;
                            break;
                        }
                        ret_report.datalen += ret;
                        break;
                    }
                }
            map++;
        }
        memcpy(out,&ret_report,sizeof(ret_report));
        return ret_report.datalen;
    }
    return -1;
}