/*	-------Structure for Leftist Tree Node-------	*/
struct treenode {
  int s, value, level;
  int vertex_number;
  int dummy;
  treenode *parent;
  treenode *left;
  treenode *right;
};


/*	-------Leftist Tree Class-------		*/
class leftist {
  private :
    treenode **node_list;   // we store a pointer for each vertex which point to it's node inside the heap
    int *dummy_list;        // dummy indication goes here
    int *inside_heap;
    int Dim;
    treenode **list_del;
  public :
    treenode* root;
    
  leftist() {
    root = NULL;
  }
	
  void init(int dim) {
    node_list = (treenode **) calloc (dim + 1, sizeof(treenode *));
    dummy_list = (int *) calloc (dim + 1, sizeof(int));
    for (int i = 0; i <= dim; i++)
      dummy_list[i] = 0;
    for (int i = 0; i <= dim; i++)
      node_list[i] = NULL;
    inside_heap = (int *) calloc (dim + 1, sizeof(int));
    for (int i = 0; i <= dim; i++)
      inside_heap[i] = 0;
    list_del = (treenode **) calloc (Dim + 1, sizeof(treenode *));
    Dim = dim;
  }

  void print_recursive_r(treenode *node) {
    if (node == NULL)
      return;
	
    if (node == root)
      printf("root = %d key %d dummy %d parent %d\n", node->vertex_number, node->value, dummy_list[node->vertex_number], node->parent ? node->parent->vertex_number : 0);
    else
      printf("node = %d key %d dummy %d parent %d\n", node->vertex_number, node->value, dummy_list[node->vertex_number], node->parent ? node->parent->vertex_number : 0);

    if (inside_heap[node->vertex_number] == 0)
      printf("BUG: inside_heap[node->vertex_number] == 0\n");

    if (node != node_list[node->vertex_number])
      printf("BUG: node != node_list[node->vertex_number]");

    print_recursive_r(node->left);
    print_recursive_r(node->right);
  }

  void print_recursive() {
    printf("print_recursive()\n");
    treenode *node = root;
    print_recursive_r(node);
    printf("\n");
  }

  void print_recursive_linear(int dim) {
    printf("print_recursive_linear()\n");
    for (int i = 1; i <= (dim+1); i++) {
        if (inside_heap[i] == 1)
          printf("%s = %d key %d dummy %d parent %d\n", (root == node_list[i])? "root":"node",  node_list[i]->vertex_number, node_list[i]->value, dummy_list[node_list[i]->vertex_number], node_list[i]->parent ? node_list[i]->parent->vertex_number : 0);
      }
  }
    
  //Inserting into Leftist tree
  void insert(int vertex_number, int key) {
    treenode *left;
    treenode *right;
    treenode *parent;

    // the case for lazy deletion heap
    if (inside_heap[vertex_number] == 1 && dummy_list[vertex_number] == 1) {
        treenode *node = node_list[vertex_number];
        node->value = key;
        dummy_list[vertex_number] = 0;
	    
        assert(node);

        while (node->parent && node->value < node->parent->value) {
            left = node->parent->left;
            right = node->parent->right;
            parent = node->parent->parent; // look here
            int tmp_npl = node->parent->s;
            node->parent->s = node->s;
            node->s = tmp_npl;
		
            if (node->parent->left && (node->parent->left == node)) {
                // node->left == node->parent->left;
                node->parent->left = node->left;
                node->parent->right = node->right;
                if (node->parent->parent)
                  {
                    if (node->parent->parent->left == node->parent)
                      {
                        node->parent->parent->left = node;
                      }
                    else
                      {
                        node->parent->parent->right = node;
                      }
                  }
                    
                if (node->left)
                  node->left->parent = node->parent;
                if (node->right)
                  node->right->parent = node->parent;
                    
                node->left = node->parent;
                node->right = right;
		    
                node->parent->parent = node;
                node->parent = parent;
		    
              } else {
                assert(node->parent->right->value == node->value);
                printf("%d %d", node->parent->right->value, node->value);

                node->parent->left = node->left;
                node->parent->right = node->right;
                if (node->parent->parent) {
                    if (node->parent->parent->left == node->parent) {
                        node->parent->parent->left = node;
                    } else {
                        assert (node->parent->parent->right == node->parent);
                        node->parent->parent->right = node;
                      }
                  }

                if (node->left)
                  node->left->parent = node->parent;
                if (node->right)
                  node->right->parent = node->parent;
		    
                node->left = left;
                node->right = node->parent;

                node->parent->parent = node;
                node->parent = parent;
              }
            if (node->parent == NULL)
              root = node;

          }

      } else {
        assert(inside_heap[vertex_number] == 0);
        treenode* node = new treenode;
        node->value=key;
        node->left=NULL;
        node->right=NULL;
        node->parent = NULL;
        node->vertex_number = vertex_number;
        node->s=1;
        node_list[vertex_number] = node;
        inside_heap[vertex_number] = 1;
        root = merge(root,node);
      }
  }
  
  //Merging Leftist tree nodes
  treenode* merge(treenode* nodeA, treenode* nodeB) {
    treenode* temp;

    if((nodeA == NULL) && (nodeB != NULL))
      return nodeB;
    else if((nodeB == NULL) && (nodeA != NULL))
      return nodeA;
    else if((nodeB == NULL) && (nodeA == NULL))
      return nodeA;
    else if(nodeA->value > nodeB->value || ((nodeA->value ==  nodeB->value) && nodeA->vertex_number < nodeB->vertex_number)) {
        temp=nodeA;
        nodeA=nodeB;
        nodeB=temp;
      }

    nodeA->right = merge(nodeA->right,nodeB);
    if (nodeA->right == nodeB)
      nodeB->parent = nodeA;

    if((nodeA->left == NULL)&&(nodeA->right!=NULL)) {
        nodeA->left=nodeA->right;
        nodeA->right=NULL;
        nodeA->s=1;
    } else {
        if(nodeA->left->s < nodeA->right->s) {
            temp = nodeA->left;
            nodeA->left=nodeA->right;
            nodeA->right=temp;
        }
        nodeA->s=nodeA->right->s+1;
    }
    return nodeA;
  }
  
  //Display function for Leftist tree
  void display() {
    if(root!=NULL) {
        root->level=0;
        levelorder(root);
    }	
  }
  
  //Level order traversal for Leftist tree
  void levelorder(treenode* t) {      
    treenode* a[15000];
    treenode* c;
    ofstream f2("result_left.txt",ios::out);
		
    for(int i=0;i<15000;i++)
      a[i]=NULL;
    int size=0,x;
    int f=0;
    if(t==NULL){ 
        f2<<"NULL TREE : No Elements";
    } else {	
        while(t!=NULL) {                                     
            f2<<t->value<<" ";
            x=t->level;
                 
            if(t->left!=NULL) {      
                a[size++]=t->left;
                t->left->level=x+1;
            }

            if(t->right!=NULL) {     
                a[size++]=t->right;
                t->right->level=x+1;
            }

            c=a[f];
            if(c!=NULL) {
                if(t->level!=c->level)
                  f2<<endl;
                else if(t->level==c->level)
                  f2<<" ";
            } else 
              f2<<endl;
              t=a[f++];
          }
    }
    f2.close();                  
  }	


  int delete_r(treenode *node) {
      int ret;
    
    if (node->left == NULL && node->right == NULL) {
	     delete node;
	     return 1;
    } else {
	     if (node->right) {
	         ret = delete_r(node->right);
	         if (ret)
		          return ret;
      } if (node->left) {
	      ret = delete_r(node->left);
	      if (ret)
		      return ret;
      }
	   return 0;
    }
  }

  int delete_all() {
      int ret = 1;
      if (root != NULL)
	       ret = delete_r(root);
      free(node_list);
      free(inside_heap);
      free(dummy_list);
      free(list_del);

      return ret;
	  
  }
  
  int remove_lazy(int *vertex_number, int *key) {
    if (inside_heap[*vertex_number] == 0)
      return 0;
	
    if (inside_heap[*vertex_number] == 1 && dummy_list[*vertex_number] == 1)
      return 0;
	
    dummy_list[*vertex_number] = 1;
    return 1;
  }
    
    
  int remove(int *vertex_number, int *key) {
    treenode *curr_node = node_list[*vertex_number];

    if (inside_heap[*vertex_number] == 0)
      return 0;
    inside_heap[*vertex_number] = 0;

    if (root == curr_node) {
        root = merge(root->right,root->left);
    } else {
        // garantees that this is a right node of someone
        if (curr_node->parent->right != curr_node) {
            if(curr_node != curr_node->parent->left) {
                printf("curr_node v=%d key=%d\n", curr_node->vertex_number, curr_node->value);
                printf("curr_node->parent v=%d key=%d\n", curr_node->parent->vertex_number, curr_node->parent->value);
                printf("curr_node->parent->left v=%d key=%d\n", curr_node->parent->left->vertex_number, curr_node->parent->left->value);
                printf("curr_node->parent->right v=%d key=%d\n", curr_node->parent->right->vertex_number, curr_node->parent->right->value);
            }
            treenode *tmp = curr_node->parent->right;
            curr_node->parent->right = curr_node->parent->left;
            curr_node->parent->left = tmp;
          }

        curr_node->parent->right = curr_node->left;
        if (curr_node->left != NULL)
          curr_node->left->parent = curr_node->parent;
	    
        treenode *node = curr_node->parent;
        // while not at the top of the tree
        do {
          // calculates the NPL of the left and right subtree
          int l = (node->left == NULL ? 0 : 
                   node->left->s);
          int r = (node->right == NULL ? 0 :
                   node->right->s);
		
          // if the left subtree has a lower NPL than
          // the right one, swaps them
          if (l < r) {
              treenode *tmp = node->right;
              node->right = node->left;
              node->left = tmp;
            }
		
          // if the null path lenght hasn't changed, stop
          if (node->s == r+1)
            break;
		
          // adjusts the null path length
          node->s = r+1;
	    
          // if this is the root node, breaks
          if (node->parent == NULL)
            break;
		
          // moves to the parent node
          node = node->parent;
        } while (1);
        merge(node,curr_node->right);
	     delete curr_node;
      }
    return 1;
  }

  void deletemin_lazy_r(treenode *node, int *i) {
    if (node == NULL)
      return;
	
    if (inside_heap[node->vertex_number] == 1 && dummy_list[node->vertex_number] == 1) {
        list_del[*i] = node;
        (*i)++;
        dummy_list[node->vertex_number] = 0;
	    
        if (node->left)
          deletemin_lazy_r(node->left, i);
        if (node->right)
          deletemin_lazy_r(node->right, i);
        return;
    } else  {
        if (node->left)
          deletemin_lazy_r(node->left, i);
        if (node->right)
          deletemin_lazy_r(node->right, i);
        return;
      }
    list_del[*i] = 0;

    *i = 0;
    while (list_del[*i] != 0) {
        remove(&node_list[*i]->vertex_number, &node_list[*i]->value);	
    }
  }
    
  int deletemin_lazy(int *vertex_number, int *key) {
    /* make a list in preorder traversal of each deleted node and delete them */
    if (root == NULL)
      return 0;
	
    int i = 0;
    deletemin_lazy_r(root, &i);

    while (root->parent != NULL)
      root = root->parent;
	
    if (root) {
        *vertex_number = root->vertex_number;
        *key = root->value;
        inside_heap[*vertex_number] = 0;
        dummy_list[*vertex_number] = 0;
        root = merge(root->right,root->left);
        if (root) {
            node_list[root->vertex_number] = root;
            root->parent = NULL;
        }
        return 1;
      }
    return 0;
  }

  // Deleting the minimum element (root) function for Leftist tree
  int deletemin(int *vertex_number, int *key)	{
    if (root) {
        *vertex_number = root->vertex_number;
        *key = root->value;
        inside_heap[*vertex_number] = 0;
        root = merge(root->right,root->left);
        if (root)
          root->parent = NULL;
        return 1;
      }
    return 0;
  }
};
