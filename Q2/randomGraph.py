import random

# Function to generate a random graph and save it to a file
def generate_graph_file(num_vertices, num_edges, filename):
    with open(filename, 'w') as f:
        f.write(f"{num_vertices} {num_edges}\n")  # Write the number of vertices and edges
        edges = set()  # Use a set to store edges and ensure uniqueness
        while len(edges) < num_edges:
            u = random.randint(1, num_vertices)  # Randomly select a vertex u
            v = random.randint(1, num_vertices)  # Randomly select a vertex v
            if u != v:  # Ensure there are no self-loops
                edge = (min(u, v), max(u, v))  # Ensure the smaller vertex comes first
                edges.add(edge)  # Add the edge to the set
        
        for edge in edges:
            f.write(f"{edge[0]} {edge[1]}\n")  # Write each edge to the file

# Parameters for the graph generation
num_vertices = 5  # Change this to the desired number of vertices
num_edges = 5     # Change this to the desired number of edges
filename = "graph.txt"  # The name of the file to save the graph

# Generate the graph and save it to a file
generate_graph_file(num_vertices, num_edges, filename)
print(f"Graph file '{filename}' generated with {num_vertices} vertices and {num_edges} edges.")
