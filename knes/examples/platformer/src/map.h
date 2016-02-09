#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#define MAP_SIZE    2970
extern byte const src_map_exported_map[MAP_SIZE];

#define MAP_W      src_map_exported_map[0]
#define MAP_H      src_map_exported_map[1]
#define MAP_DATA   (src_map_exported_map+2)

#define MAP_MET_SIZE    1024
extern byte const src_map_exported_metatiles[MAP_MET_SIZE];

#define MET0       (src_map_exported_metatiles+0)
#define MET1       (src_map_exported_metatiles+256)
#define MET2       (src_map_exported_metatiles+512)
#define MET3       (src_map_exported_metatiles+768)

#define MAP_ATTRIB_SIZE    256
extern byte const src_map_exported_attrib[MAP_ATTRIB_SIZE];

#endif //!MAP_H_INCLUDED
