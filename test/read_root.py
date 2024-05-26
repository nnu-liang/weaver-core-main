import uproot
import math


def read_root(filepath, branches, load_range=None, treename=None, branch_magic=None):
    with uproot.open(filepath) as f:
        if treename is None:
            treenames = set([k.split(';')[0] for k, v in f.items() if getattr(v, 'classname', '') == 'TTree'])
            if len(treenames) == 1:
                treename = treenames.pop()
            else:
                raise RuntimeError(
                    'Need to specify `treename` as more than one trees are found in file %s: %s' %
                    (filepath, str(treenames)))
        tree = f[treename]
        if load_range is not None:
            start = math.trunc(load_range[0] * tree.num_entries)
            stop = max(start + 1, math.trunc(load_range[1] * tree.num_entries))
        else:
            start, stop = None, None
        if branch_magic is not None:
            branch_dict = {}
            for name in branches:
                decoded_name = name
                for src, tgt in branch_magic.items():
                    if src in decoded_name:
                        decoded_name = decoded_name.replace(src, tgt)
                branch_dict[name] = decoded_name
            outputs = tree.arrays(filter_name=list(branch_dict.values()), entry_start=start, entry_stop=stop)
            for name, decoded_name in branch_dict.items():
                if name != decoded_name:
                    outputs[name] = outputs[decoded_name]
        else:
            outputs = tree.arrays()
            outputs = tree.arrays(filter_name=branches, entry_start=start, entry_stop=stop)
    return outputs


if __name__ == '__main__':
    file_name1 = './data/val_5M/HToBB_122.root'
    file_name2 = './data/val_5M/TTBar_120.root'
    aaa = read_root(file_name1, {'part_py', 'label_Hbb'})
    print("数据：", aaa)
    print((aaa[3]))
    print(type(aaa))
    print(type(aaa[0]))
    print("数据样本个数（每个文件）：", len(aaa))
    print(aaa[0]['label_Hbb'])
    print("每个特征数据维度：", len(aaa[1]['part_py']))
    print(aaa.fields)
    print("有多少个候选特征：", len(aaa))
