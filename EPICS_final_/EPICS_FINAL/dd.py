from datasets import load_dataset
# Login using e.g. `huggingface-cli login` to access this dataset
ds = load_dataset("MaxPrestige/CA_Weather_Fire_Dataset_Cleaned")
print(ds)