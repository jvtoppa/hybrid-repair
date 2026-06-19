import subprocess
import os

def experiment():
    base_dataset_dir = '../datasets/'
    
    output_base_dir = os.path.join(os.getcwd(), 'output_reports')
    os.makedirs(output_base_dir, exist_ok=True)
    
        
    for root, dirs, files in os.walk(base_dataset_dir):
        
        for filename in files:
    
            dataset_path = os.path.join(root, filename)
            category = "non-repetitive" if "non-repetitive" in root else "repetitive" if "repetitive" in root else "unknown"
            
            relative_path = os.path.relpath(root, base_dataset_dir)
            dataset_output_dir = os.path.join(output_base_dir, relative_path)
            os.makedirs(dataset_output_dir, exist_ok=True)
            
            log_file_path = os.path.join(dataset_output_dir, f"{filename}_output.txt")

            print(f"Profiling Algorithm: {filename}")
            
            with open(log_file_path, "w") as log_file:
    
                try:
                    subprocess.run(f"../build/hybrid_fast < {dataset_path}", stdout=log_file, stderr=log_file, shell=True, timeout=1800)
    
                except subprocess.TimeoutExpired:
                    print("ERROR_TIMEOUT")
                    log_file.write("\nERROR: TimeoutExpired after 1800s\n")
    
                except Exception as e:
                    print(f"ERROR_{type(e).__name__} (gcis_fast)")
                    log_file.write(f"\nERROR: {type(e).__name__}\n")
                
if __name__ == "__main__":
    experiment()